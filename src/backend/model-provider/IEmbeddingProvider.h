#pragma once
#include "IModelProvider.h"

class ILLMInferProvider;
class ThreadPool;

class IEmbeddingProvider final: public IModelProvider {
public:
	explicit IEmbeddingProvider(size_t n_thread);
	virtual ~IEmbeddingProvider() = default;

	// Inherited via IModelProvider
	const char* name() const override;

	ModelCapability capability() const override;

	Status initialize(
		const char* model_path, 
		const Model_Params& params) override;

	Status updateParams(
		const Model_Params& params) override;

	void unInitialize() override;

	Status embedText(std::string_view text, LocalAI_EmbeddingResult* out) const;

private:
	std::unique_ptr<ILLMInferProvider> engine_;
	std::unique_ptr<ThreadPool> threads_;
};