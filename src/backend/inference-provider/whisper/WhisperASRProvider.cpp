#define _USE_MATH_DEFINES // for M_PI

#include "WhisperASRProvider.h"
#include "ggml.h"
#include "ggml-backend.h"
#include "whisper.h"

#include <chrono>
#include <fstream>
#include <sstream>
#include <regex>
#include <assert.h>

constexpr const char* WHISPER_MODEL_PATH = "models/ggml-small.en.bin";
constexpr const char* WHISPER_VAD_MODEL_PATH = "D:\\ai-projects\\LocalAI-Assistant\\models\\ggml-silero-v5.1.2.bin";
const std::string k_prompt_whisper = R"(A conversation with a person.)";

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

static void high_pass_filter(std::vector<float> &data, float cutoff, float sample_rate) 
{
    const float rc = 1.0f / (2.0f * M_PI * cutoff);
    const float dt = 1.0f / sample_rate;
    const float alpha = dt / (rc + dt);

    float y = data[0];

    for (size_t i = 1; i < data.size(); i++) {
        y = alpha * (y + data[i] - data[i - 1]);
        data[i] = y;
    }
}

static bool vad_simple(std::vector<float> &pcmf32, int sample_rate, int last_ms, float vad_thold, float freq_thold, bool verbose) 
{
    const int n_samples = pcmf32.size();
    const int n_samples_last = (sample_rate * last_ms) / 1000;

    if (n_samples_last >= n_samples) {
        // not enough samples - assume no speech
        return false;
    }

    if (freq_thold > 0.0f) {
        high_pass_filter(pcmf32, freq_thold, sample_rate);
    }

    float energy_all = 0.0f;
    float energy_last = 0.0f;

    for (int i = 0; i < n_samples; i++) {
        energy_all += fabsf(pcmf32[i]);

        if (i >= n_samples - n_samples_last) {
            energy_last += fabsf(pcmf32[i]);
        }
    }

    energy_all /= n_samples;
    energy_last /= n_samples_last;

    if (verbose) {
        fprintf(stderr, "%s: energy_all: %f, energy_last: %f, vad_thold: %f, freq_thold: %f\n", __func__, energy_all, energy_last, vad_thold, freq_thold);
    }

    if (energy_last > vad_thold * energy_all) {
        return false;
    }

    return true;
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
    const float* pcmf32,
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

    // VAD params
    wparams.vad = params.vad;
    wparams.vad_model_path = params.vad_model.c_str();

    wparams.vad_params.threshold = params.vad_threshold;
    wparams.vad_params.min_speech_duration_ms = params.vad_min_speech_duration_ms;
    wparams.vad_params.min_silence_duration_ms = params.vad_min_silence_duration_ms;
    wparams.vad_params.max_speech_duration_s = params.vad_max_speech_duration_s;
    wparams.vad_params.speech_pad_ms = params.vad_speech_pad_ms;
    wparams.vad_params.samples_overlap = params.vad_samples_overlap;

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

WhisperASRProvider::WhisperASRProvider()
{
    //params_.vad = true;
    //params_.vad_model = WHISPER_VAD_MODEL_PATH;
}

WhisperASRProvider::~WhisperASRProvider()
{
    if (ctx_wsp_) {
        whisper_free(ctx_wsp_);
        ctx_wsp_ = nullptr;
    }
}

Status WhisperASRProvider::initialize(const char* model_path, const Model_Params& params)
{
    struct whisper_context_params cparams = whisper_context_default_params();

    ctx_wsp_ = whisper_init_from_file_with_params(model_path, cparams);
    if (ctx_wsp_ == nullptr) {
        fprintf(stderr, "error: failed to initialize whisper context\n");
		return {
			LocalAI_ErrorCode::LOCALAI_MODEL_LOAD_FAILED,
		    "Failed to initialize whisper context"
        };
    }

    return {};
}

Status WhisperASRProvider::updateParams(const Model_Params& params)
{
	return Status();
}

bool WhisperASRProvider::vadSample(std::vector<float> &samples, int sample_rate, int last_ms)
{
    return vad_simple(samples, sample_rate, last_ms,
        params_.vad_thold, params_.freq_thold, params_.print_energy);
}

Status WhisperASRProvider::transcribe(
    std::span<const float> samples, std::string& text_heard)
{
    text_heard = "";
    if (!ctx_wsp_) {
        assert(false && "ASR session not started");
        return { 
            LocalAI_ErrorCode::LOCALAI_NOT_INITIALIZED,
            "ASR session not started"
        };
    }

    if (samples.empty()) {
        assert(false && "Invalid audio data");
        return { 
            LocalAI_ErrorCode::LOCALAI_INVALID_ARG, 
            "Invalid audio data" 
        };
    }

    float prob0 = 0.0f;
    int64_t t_ms = 0;
    auto all_heard = ::trim(::transcribe(ctx_wsp_, params_, samples.data(), samples.size(), k_prompt_whisper, prob0, t_ms));
    const auto words = get_words(all_heard);

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

    return {};
}
