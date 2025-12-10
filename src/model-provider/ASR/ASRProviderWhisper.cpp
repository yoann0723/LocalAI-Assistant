#include "ASRProviderWhisper.h"
#include "ggml.h"
#include "ggml-backend.h"
#include "whisper.h"
#include <future>
#include <chrono>
#include <fstream>
#include <sstream>
#include <regex>
#include <assert.h>

constexpr const char* WHISPER_MODEL_PATH = "models/ggml-small.en.bin";
const std::string k_prompt_whisper = R"(A conversation with a person called {1}.)";

// trim whitespace from the beginning and end of a string
static std::string trim(const std::string& str) {
	size_t start = 0;
	size_t end = str.size();
	while (start < end && isspace(static_cast<unsigned char>(str[start]))) {
		start += 1;
	}
	while (end > start && isspace(static_cast<unsigned char>(str[end - 1]))) {
		end -= 1;
	}
	return str.substr(start, end - start);
}

static std::vector<std::string> get_words(const std::string& txt) {
    std::vector<std::string> words;

    std::istringstream iss(txt);
    std::string word;
    while (iss >> word) {
        words.push_back(word);
    }

    return words;
}

static std::string transcribe(
    whisper_context* ctx,
    const whisper_params& params,
    const float *pcmf32,
	int64_t pcmf32_size,
    const std::string prompt_text,
    float& prob,
    int64_t& t_ms) {
    const auto t_start = std::chrono::high_resolution_clock::now();

    prob = 0.0f;
    t_ms = 0;

    std::vector<whisper_token> prompt_tokens;

    whisper_full_params wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);

    prompt_tokens.resize(1024);
    prompt_tokens.resize(whisper_tokenize(ctx, prompt_text.c_str(), prompt_tokens.data(), prompt_tokens.size()));

    wparams.print_progress = false;
    wparams.print_special = params.print_special;
    wparams.print_realtime = false;
    wparams.print_timestamps = !params.no_timestamps;
    wparams.translate = params.translate;
    wparams.no_context = true;
    wparams.single_segment = true;
    wparams.max_tokens = params.max_tokens;
    wparams.language = params.language.c_str();
    wparams.n_threads = params.n_threads;

    wparams.prompt_tokens = prompt_tokens.empty() ? nullptr : prompt_tokens.data();
    wparams.prompt_n_tokens = prompt_tokens.empty() ? 0 : prompt_tokens.size();

    wparams.audio_ctx = params.audio_ctx;

    if (whisper_full(ctx, wparams, pcmf32, pcmf32_size) != 0) {
        return "";
    }

    int prob_n = 0;
    std::string result;

    const int n_segments = whisper_full_n_segments(ctx);
    for (int i = 0; i < n_segments; ++i) {
        const char* text = whisper_full_get_segment_text(ctx, i);

        result += text;

        const int n_tokens = whisper_full_n_tokens(ctx, i);
        for (int j = 0; j < n_tokens; ++j) {
            const auto token = whisper_full_get_token_data(ctx, i, j);

            prob += token.p;
            ++prob_n;
        }
    }

    if (prob_n > 0) {
        prob /= prob_n;
    }

    const auto t_end = std::chrono::high_resolution_clock::now();
    t_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count();

    return result;
}

ASRProviderWhisper::ASRProviderWhisper()
{
	k_prompt_wsp = k_prompt_whisper;
}

ASRProviderWhisper::~ASRProviderWhisper()
{
}

ModelProvider::Result<void> ASRProviderWhisper::initialize(std::string_view model_path, Model_Params params)
{
    if (sessionStarted)
        return {};

    ggml_backend_load_all();

    struct whisper_context_params cparams = whisper_context_default_params();

    ctx_wsp = whisper_init_from_file_with_params(WHISPER_MODEL_PATH, cparams);
    if (ctx_wsp == nullptr) {
        fprintf(stderr, "error: failed to initialize whisper context\n");
        return tl::unexpected(LocalAI_ErrorCode::LOCALAI_MODEL_LOAD_FAILED);
    }

    sessionStarted = true;
    return {};
}

ModelProvider::Result<std::string> ASRProviderWhisper:: transcribe(std::span<const float> samples)
{
    if (!ctx_wsp) {
		assert(false && "ASR session not started");
        return tl::unexpected(LocalAI_ErrorCode::LOCALAI_NOT_INITIALIZED);
    }

    if (samples.empty()) {
		assert(false && "Invalid audio data");
		return tl::unexpected(LocalAI_ErrorCode::LOCALAI_INVALID_ARG);;
    }

    float prob0 = 0.0f;
    int64_t t_ms = 0;
    auto all_heard = ::trim(::transcribe(ctx_wsp, params, samples.data(), samples.size(), k_prompt_wsp, prob0, t_ms));
    const auto words = get_words(all_heard);

    std::string text_heard;

    for (int i = 0; i < (int)words.size(); ++i) {
        text_heard += words[i] + " ";
    }

    // remove text between brackets using regex
    {
        std::regex re("\\[.*?\\]");
        text_heard = std::regex_replace(text_heard, re, "");
    }

    // remove text between brackets using regex
    {
        std::regex re("\\(.*?\\)");
        text_heard = std::regex_replace(text_heard, re, "");
    }

    // remove all characters, except for letters, numbers, punctuation and ':', '\'', '-', ' '
    text_heard = std::regex_replace(text_heard, std::regex("[^a-zA-Z0-9åäöÅÄÖ\\.,\\?!\\s\\:\\'\\-]"), "");

    // take first line
    text_heard = text_heard.substr(0, text_heard.find_first_of('\n'));

    // remove leading and trailing whitespace
    text_heard = std::regex_replace(text_heard, std::regex("^\\s+"), "");
    text_heard = std::regex_replace(text_heard, std::regex("\\s+$"), "");

	return text_heard;
}