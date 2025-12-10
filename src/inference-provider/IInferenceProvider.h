#pragma once
#include "../core/core_c_api.h"

class IInferenceProvider {
public:
    virtual ~IInferenceProvider() = default;

    // Load model, allocate backend resources
    virtual LocalAI_Status* initialize(
        const char* model_path,
        const Model_Params& params) = 0;

    // Update runtime parameters (temperature, threads, etc.)
    virtual LocalAI_Status* updateParams(
        const Model_Params& params) = 0;

    // Submit an async inference task
    virtual LocalAI_Status* submit(
        const InferenceRequest& request,
        InferenceCallback callback,
        void* user_data) = 0;
};
