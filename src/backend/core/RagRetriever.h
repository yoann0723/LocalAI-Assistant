#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "AIModelHub.h"
#include "IEmbeddingProvider.h"

struct CapabilityInfo {
    std::string name;
    std::string description;
    std::vector<float> embedding;
};

class RagRetriever {
public:
    RagRetriever() = default;
    RagRetriever(const IEmbeddingProvider* embProvider, size_t topK = 5);

    void setEmbeddingProvider(const IEmbeddingProvider* embProvider);

    void setTopK(size_t topk);

    void loadCapabilities(const std::vector<CapabilityInfo>& caps);

    // inpuit = current context(multi-turn) + user query
    std::vector<CapabilityInfo> retrieve(std::string_view query);

private:
    bool embed(std::string_view text, LocalAI_EmbeddingResult* output);
    float cosineSimilarity(std::span<const float> a,
                           const std::vector<float>& b);

private:
    size_t topK_;
    std::vector<CapabilityInfo> capabilities_;
    const IEmbeddingProvider* embProvider_ = nullptr;
};
