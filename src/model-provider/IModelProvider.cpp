#include "IModelProvider.h"
#include "./LLM/LLMProviderLlama.h"
#include "./STT/STTProviderWhisper.h"
#include "./vision/VisionProvider.h"
#include "./embedding/EmbeddingProviderSbert.h"

MODEL_PROVIDER_API std::shared_ptr<ILLMProvider> createLLMProvider()
{
    return std::make_shared<LLMProviderLlama>();
}

MODEL_PROVIDER_API std::shared_ptr<IEmbeddingProvider> createEmbeddingProvider()
{
    return std::make_shared<EmbeddingProviderSbert>();
}

MODEL_PROVIDER_API std::shared_ptr<ISTTProvider> createSTTProvider()
{
    return std::make_shared<STTProviderWhisper>();
}

MODEL_PROVIDER_API std::shared_ptr<IVisionProvider> createVisionProvider()
{
    return std::make_shared<VisionProvider>();
}
