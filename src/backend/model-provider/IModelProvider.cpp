#include "IModelProvider.h"
#include "ILLMProvider.h"
#include "IEmbeddingProvider.h"
#include "IASRProvider.h"
#include "IVisionProvider.h"
#include "ICustomModelProvider.h"
#include "IInferenceProvider.h"
#include "ThreadPool.h"

std::unique_ptr<IModelProvider> ModelProviderFactory::createBuildInModel(
	BuildInModelType type, size_t n_thread)
{
	switch (type) {
	case BuildInModelType::LLM:
		return std::make_unique<ILLMProvider>();
	case BuildInModelType::Embedding:
		return std::make_unique<IEmbeddingProvider>(n_thread);
	case BuildInModelType::ASR:
		return std::make_unique<IASRProvider>();
	case BuildInModelType::Vision:
		return std::make_unique<IVisionProvider>(n_thread);
	default:
		break;
	}

    return nullptr;
}

std::unique_ptr<IModelProvider> ModelProviderFactory::createCustomModel( 
    InferenceType infernce_type, size_t n_thread)
{
    return std::make_unique<ICustomModelProvider>(n_thread);
}
