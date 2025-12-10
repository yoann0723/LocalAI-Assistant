#pragma once
#include "../IInferenceProvider.h"
#include "llama.h"
#include <memory>

struct llama_model;
struct llama_context;

class LlamaInferenceProvider final : public IInferenceProvider {
public:
    LlamaInferenceProvider();
    ~LlamaInferenceProvider() override;

    LocalAI_Status* initialize(
        const char* model_path,
        const Model_Params& params) override;

    LocalAI_Status* updateParams(
        const Model_Params& params) override;

    LocalAI_Status* submit(
        const InferenceRequest& request,
        InferenceCallback callback,
        void* user_data) override;

private:
    llama_model* model_ = nullptr;
    llama_context* ctx_ = nullptr;

    // Typically single-thread or limited concurrency
    // You can plug your own ThreadPool here
};
