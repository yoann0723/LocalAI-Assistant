#include "LLMProviderLlama.h"
#include <assert.h>

ModelProvider::Result<std::string> LLMProviderLlama::generate(std::string_view prompt, size_t max_len)
{
	if (prompt.empty() || max_len == 0) {
		printf("Prompt is null or empty");
		return tl::unexpected(LocalAI_ErrorCode::LOCALAI_INVALID_ARG);
	}
	//TODO: call Llama model here to generate text
	return generatedText_;
}
