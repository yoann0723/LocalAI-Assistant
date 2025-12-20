#pragma once
#include "IModelProvider.h"
#include <vector>

class EmbeddingProviderSbert : public ModelProvider::IEmbeddingProvider {
public:
	virtual ~EmbeddingProviderSbert() = default;
	virtual ModelProvider::Result<std::vector<float>> embedText(std::string_view text) override;
private:
	std::vector<float> embedding_;
};