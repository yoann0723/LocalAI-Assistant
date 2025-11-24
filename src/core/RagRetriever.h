#pragma once
#include <string>
#include <vector>
#include <unordered_map>

struct CapabilityInfo {
    std::string name;
    std::string description;
    std::vector<float> embedding;
};

class RagRetriever {
public:
    RagRetriever(size_t topK);

    void loadCapabilities(const std::vector<CapabilityInfo>& caps);

    // inpuit = current context(multi-turn) + user query
    std::vector<CapabilityInfo> retrieve(const std::string& query);

private:
    std::vector<float> embed(const std::string& text);
    float cosineSimilarity(const std::vector<float>& a,
                           const std::vector<float>& b);

private:
    size_t topK_;
    std::vector<CapabilityInfo> capabilities_;
};
