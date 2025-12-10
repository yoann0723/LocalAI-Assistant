#pragma once
#include "IModelProvider.h"
#include <future>

class LLMProviderLlama : public ModelProvider::ILLMProvider {
public:
	virtual ~LLMProviderLlama() = default;

	virtual ModelProvider::Result<std::string> generate(std::string_view prompt, size_t max_len) override;

private:
	std::string generatedText_;
};