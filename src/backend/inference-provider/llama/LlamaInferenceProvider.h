#pragma once
#include "../IInferenceProvider.h"
#include "llama-cpp.h"
#include "../common/common.h"
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

    //Status generate(
	//	std::string_view prompt, LocalAI_TextResult_t** output) override;

    Status generate(
        std::string_view prompt, LLMOutput** output) override;

private:
    llama_sampler_ptr smpl_;
    llama_context_ptr ctx_;
    llama_model_ptr model_;
    std::vector<llama_chat_message> messages_;
    std::vector<char> formatted_;
    int prev_len = 0;
};
