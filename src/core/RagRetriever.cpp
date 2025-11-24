#include "RagRetriever.h"
#include "AIModelHub.h"

RagRetriever::RagRetriever(size_t topK)
    : topK_(topK) {}

void RagRetriever::loadCapabilities(const std::vector<CapabilityInfo>& caps) {
    capabilities_ = caps;
}

std::vector<CapabilityInfo> RagRetriever::retrieve(const std::string& query) {
    auto queryEmbedding = embed(query);

    // Compute similarities
    std::vector<std::pair<size_t, float>> sims; // index, similarity
    for (size_t i = 0; i < capabilities_.size(); ++i) {
        float sim = cosineSimilarity(queryEmbedding, capabilities_[i].embedding);
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
    return results;
}

std::vector<float> RagRetriever::embed(const std::string& text) {
    auto embeddingProvider = AIModelHub::instance().embeddingModelProvider();
    auto futureResult = embeddingProvider->embedText(text);
    auto inferenceResult = futureResult.get();
    if (!inferenceResult.ok) {
        return {};
    }
    return inferenceResult.embedding;
}

float RagRetriever::cosineSimilarity(const std::vector<float>& a,
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