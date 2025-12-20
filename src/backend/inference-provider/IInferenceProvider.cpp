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
    return std::unique_ptr<WhisperASRProvider>();
}

std::unique_ptr<IVisionInferProvider> InferenceFactory::createVisionInfer()
{
    return std::unique_ptr<IVisionInferProvider>();
}
