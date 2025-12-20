#include "RagRetriever.h"
#include "AIModelHub.h"
#include "IEmbeddingProvider.h"

RagRetriever::RagRetriever(AIModelHub* model_hubs, size_t topK)
    :model_hub_(model_hubs), topK_(topK) {}

void RagRetriever::loadCapabilities(const std::vector<CapabilityInfo>& caps) {
    capabilities_ = caps;
}

std::vector<CapabilityInfo> RagRetriever::retrieve(std::string_view query) {

    LocalAI_EmbeddingResult out;
    auto ok = embed(query, &out);
    if (!ok) {
		fprintf(stderr, "Failed to embed query for RAG retrieval.\n");
        return {};
	}

	std::span<const float> embeddings(out.data, out.length);

    // Compute similarities
    std::vector<std::pair<size_t, float>> sims; // index, similarity
    for (size_t i = 0; i < capabilities_.size(); ++i) {
        float sim = cosineSimilarity(embeddings, capabilities_[i].embedding);
        sims.emplace_back(i, sim);
    }

    // Sort by similarity
    std::sort(sims.begin(), sims.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    // Select topK
    std::vector<CapabilityInfo> results;
    for (size_t i = 0; i < std::min(topK_, sims.size()); ++i) {
        results.push_back(capabilities_[sims[i].first]);
    }

	LocalAI_EmbeddingResult_Free(&out);
    return results;
}

bool RagRetriever::embed(std::string_view text, LocalAI_EmbeddingResult* output)
{
    auto embeddingProvider = model_hub_->modelProvider<IEmbeddingProvider>(Model_Type::LOCALAI_MODEL_EMBEDDING);
    auto status = embeddingProvider->embedText(text, output);
    return status;
}

float RagRetriever::cosineSimilarity(std::span<const float> a,
                                     const std::vector<float>& b) {
    if (a.size() != b.size() || a.empty()) {
        return 0.0f;
    }
    float dot = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    if (normA == 0.0f || normB == 0.0f) {
        return 0.0f;
    }
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}