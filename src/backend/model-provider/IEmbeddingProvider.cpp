#include "IEmbeddingProvider.h"
#include "IInferenceProvider.h"
#include "ThreadPool.h"

IEmbeddingProvider::IEmbeddingProvider(size_t n_thread)
	: engine_(std::move(InferenceFactory::createLLMInfer())),
	threads_(std::make_unique<ThreadPool>(n_thread == 0 ? 2 : n_thread))
{
}

const char* IEmbeddingProvider::name() const
{
	return "embedding";
}

ModelCapability IEmbeddingProvider::capability() const
{
	return ModelCapability::Embedding;
}

Status IEmbeddingProvider::initialize(const char* model_path, const Model_Params& params)
{
	return {};
}

Status IEmbeddingProvider::updateParams(const Model_Params& params)
{
	return {};
}

void IEmbeddingProvider::unInitialize()
{
}

Status IEmbeddingProvider::embedText(std::string_view text, LocalAI_EmbeddingResult* out) const
{
	if (text.empty()) {
		printf("Invalid text input for embedding.\n");
		return {
			LocalAI_ErrorCode::LOCALAI_INVALID_ARG,
			"Invalid text input for embedding."
		};
	}

	out->data = nullptr;
	out->length = 0;

	return {};
}
