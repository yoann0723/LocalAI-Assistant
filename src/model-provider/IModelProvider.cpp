#include "IModelProvider.h"
#include "./LLM/LLMProviderLlama.h"
#include "./ASR/ASRProviderWhisper.h"
#include "./vision/VisionProvider.h"
#include "./embedding/EmbeddingProviderSbert.h"

namespace ModelProvider {
    std::unique_ptr<ModelProviderBase> createLLMProvider()
    {
        return std::make_unique<LLMProviderLlama>();
    }

    std::unique_ptr<ModelProviderBase> createEmbeddingProvider()
    {
        return std::make_unique<EmbeddingProviderSbert>();
    }

    std::unique_ptr<ModelProviderBase> createASRProvider()
    {
        return std::make_unique<ASRProviderWhisper>();
    }

    std::unique_ptr<ModelProviderBase> createVisionProvider()
    {
        return std::make_unique<VisionProvider>();
    }
}
