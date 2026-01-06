#include "IInferenceProvider.h"
#include "llama/LlamaInferenceProvider.h"
#include "onnxruntime/OrtInferenceProvider.h"
#include "whisper/WhisperASRProvider.h"

std::unique_ptr<ITensorInferProvider> InferenceFactory::createTensorInfer()
{
    return std::make_unique<OrtInferenceSession>();
}

std::unique_ptr<ILLMInferProvider> InferenceFactory::createLLMInfer()
{
    return std::make_unique<LlamaInferenceProvider>();
}

std::unique_ptr<IASRInferProvider> InferenceFactory::createASRInfer()
{
    auto infer = std::make_unique<WhisperASRProvider>();
    fprintf(stderr, "%s: created provider: %p", __FUNCTION__, (void*)infer.get());
    return infer;
}

std::unique_ptr<IVisionInferProvider> InferenceFactory::createVisionInfer()
{
    //return std::make_unique<IVisionInferProvider>();
    //TODO: not implemented
    return {};
}

void BackendLoader::loadBackend()
{
    static bool initialized_ggml_back_end = []() {
        ggml_backend_load_all();
        return true;
    }();
}
