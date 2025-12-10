#include "EmbeddingProviderSbert.h"

ModelProvider::Result<std::vector<float>> EmbeddingProviderSbert::embedText(std::string_view text)
{
	if (text.empty()) {
		printf("Invalid text input for embedding.\n");
		return tl::unexpected(LocalAI_ErrorCode::LOCALAI_INVALID_ARG);
	}
	//TODO: call SBERT embedding model here
	return embedding_;
}
