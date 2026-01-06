#pragma once
#include "localai_c_api.h"
#include "../core/core_common.h"
#include "../common/common.hpp"
#include "IInferenceProvider.h"
#include <span>

typedef void(*InferenceCallback)(Status status, Variant *result, void* user_data);

struct InferenceRequest {
    Variant data;
    InferenceCallback callback;
    void* user_data;
};

class ITensorInferProvider {
public:
    virtual ~ITensorInferProvider() = default;

    // Load model, allocate backend resources
    virtual Status initialize(
        const char* model_path,
        const Model_Params& params) = 0;

    // Update runtime parameters (temperature, threads, etc.)
    virtual Status updateParams(
        const Model_Params& params) = 0;

    // Submit an sync inference task
    virtual Status run(
        const InferenceRequest& request,
        Variant* result) = 0;

    // Submit an async inference task
    virtual Status runAsync(
        const InferenceRequest& request,
        InferenceCallback callback,
        void* user_data) = 0;
};

class ILLMInferProvider {
public:
    virtual ~ILLMInferProvider() = default;

    // Load model, allocate backend resources
    virtual Status initialize(
        const char* model_path,
        const Model_Params& params) = 0;

    // Update runtime parameters (temperature, threads, etc.)
    virtual Status updateParams(
        const Model_Params& params) = 0;

    virtual Status generate(
        std::string_view prompt, LLMOutput** output) = 0;

    virtual void stopGenerate() = 0;
};

class IASRInferProvider {
public:
    virtual ~IASRInferProvider() = default;

    // Load model, allocate backend resources
    virtual Status initialize(
        const char* model_path,
        const Model_Params& params) = 0;

    // Update runtime parameters (temperature, threads, etc.)
    virtual Status updateParams(
        const Model_Params& params) = 0;

    virtual bool vadSample(std::vector<float> &samples, int sample_rate, int last_ms) = 0;

    virtual Status transcribe(
        std::span<const float> samples, std::string& out) = 0;
};

class IVisionInferProvider {
public:
    virtual ~IVisionInferProvider() = default;

    // Load model, allocate backend resources
    virtual Status initialize(
        const char* model_path,
        const Model_Params& params) = 0;

    // Update runtime parameters (temperature, threads, etc.)
    virtual Status updateParams(
        const Model_Params& params) = 0;

    virtual std::string transcribe(
        const std::string& prompt) = 0;
};

class InferenceFactory {
public:
    static std::unique_ptr<ITensorInferProvider> createTensorInfer();
    static std::unique_ptr<ILLMInferProvider> createLLMInfer();
    static std::unique_ptr<IASRInferProvider> createASRInfer();
    static std::unique_ptr<IVisionInferProvider> createVisionInfer();
};

class BackendLoader {
public:
    static void loadBackend();
};
