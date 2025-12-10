#pragma once
#include "IModelProvider.h"

// command-line parameters
struct whisper_params {
    int32_t n_threads = std::min(4, (int32_t)std::thread::hardware_concurrency());
    int32_t voice_ms = 10000;
    int32_t capture_id = -1;
    int32_t max_tokens = 32;
    int32_t audio_ctx = 0;
    int32_t n_gpu_layers = 999;
    int32_t seed = 0;
    int32_t top_k = 5;
    int32_t min_keep = 1;
    float top_p = 0.80f;
    float min_p = 0.01f;
    float temp = 0.30f;

    float vad_thold = 0.6f;
    float freq_thold = 100.0f;

    bool translate = false;
    bool print_special = false;
    bool print_energy = false;
    bool no_timestamps = true;
    bool verbose_prompt = false;
    bool use_gpu = true;
    bool flash_attn = true;

    std::string person = "Georgi";
    std::string bot_name = "LLaMA";
    std::string wake_cmd = "";
    std::string heard_ok = "";
    std::string language = "en";
    std::string model_wsp = "models/ggml-base.en.bin";
    std::string model_llama = "models/ggml-llama-7B.bin";
    std::string speak = "./examples/talk-llama/speak";
    std::string speak_file = "./examples/talk-llama/to_speak.txt";
    std::string prompt = "";
    std::string fname_out;
    std::string path_session = "";       // path to file for saving/loading model eval state
};

class ASRProviderWhisper : public ModelProvider::IASRProvider {
public:
	ASRProviderWhisper();
	virtual ~ASRProviderWhisper();

    virtual ModelProvider::Result<void> initialize(std::string_view model_path, Model_Params params) override;
	virtual ModelProvider::Result<std::string> transcribe(std::span<const float> samples) override;

private:
	bool sessionStarted = false;
	struct whisper_context* ctx_wsp = nullptr;
	whisper_params params;
	std::string k_prompt_wsp;
	std::string transcribedText_;
};