#pragma once
#include "IModelProvider.h"
#include <future>

class EmbeddingProviderSbert : public IEmbeddingProvider {
	public:
	virtual ~EmbeddingProviderSbert() = default;
	// Asynchronous call returning future
	virtual std::future<InferenceResult> embedText(const std::string& text) override {
		return std::async(std::launch::async, [text]() {
			// Placeholder implementation
			InferenceResult result;
			result.ok = true;
			// Dummy embedding: vector of fixed size with incremental values
			const int embeddingSize = 768;
			result.embedding.resize(embeddingSize);
			for (int i = 0; i < embeddingSize; ++i) {
				result.embedding[i] = static_cast<float>(i) / embeddingSize;
			}
			return result;
		});
	}
};