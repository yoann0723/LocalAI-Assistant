#pragma once
#include "../IInferenceProvider.h"
#include "llama-cpp.h"
#include <memory>

struct llama_model;
struct llama_context;

class LlamaInferenceProvider final : public ILLMInferProvider {
public:
    LlamaInferenceProvider();
    ~LlamaInferenceProvider() override;

    Status initialize(
        const char* model_path,
        const Model_Params& params) override;

    Status updateParams(
        const Model_Params& params) override;

    Status generate(
        std::string_view prompt, std::string &response) override;

private:
    llama_model_ptr model_;
    llama_context_ptr ctx_;
    llama_sampler_ptr smpl_;
};
