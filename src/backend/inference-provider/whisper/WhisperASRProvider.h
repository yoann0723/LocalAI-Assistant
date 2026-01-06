#pragma once
#include <memory>
#include <thread>

#include "whisper.h"
#include "../IInferenceProvider.h"

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

    // Voice Activity Detection (VAD) parameters
    bool        vad = false;
    std::string vad_model = "";
    float       vad_threshold = 0.5f;
    int         vad_min_speech_duration_ms = 250;
    int         vad_min_silence_duration_ms = 100;
    float       vad_max_speech_duration_s = FLT_MAX;
    int         vad_speech_pad_ms = 30;
    float       vad_samples_overlap = 0.1f;
};

struct llama_model;
struct llama_context;

class WhisperASRProvider final : public IASRInferProvider {
public:
    WhisperASRProvider();
    ~WhisperASRProvider() override;

    Status initialize(
        const char* model_path,
        const Model_Params& params) override;

    Status updateParams(
        const Model_Params& params) override;

    bool vadSample(std::vector<float> &samples, int sample_rate, int last_ms) override;

    Status transcribe(
        std::span<const float> samples, 
        std::string& out) override;

private:
    struct whisper_context *ctx_wsp_ = nullptr;
    whisper_params params_;

    // Typically single-thread or limited concurrency
    // You can plug your own ThreadPool here
};

