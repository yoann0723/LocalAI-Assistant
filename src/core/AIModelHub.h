#pragma once
#include "IModelProvider.h"
#include "CommonDef.h"
#include <memory>
#include <mutex>

// Simple model hub that owns a concrete IModelProvider instance.
class CORE_API AIModelHub {
public:
    static AIModelHub& instance();

    // initialize with a concrete provider (calls should be thread-safe)
    void setTextModelProvider(std::shared_ptr<ILLMProvider> provider);
    std::shared_ptr<ILLMProvider> textModelProvider();

	void setEmbeddingModelProvider(std::shared_ptr<IEmbeddingProvider> provider);
	std::shared_ptr<IEmbeddingProvider> embeddingModelProvider();

	void setVisionModelProvider(std::shared_ptr<IVisionProvider> provider);
	std::shared_ptr<IVisionProvider> visionModelProvider();

private:
    AIModelHub() = default;
    std::mutex mtx_;
    std::shared_ptr<ILLMProvider> textProvider_;
	std::shared_ptr<IEmbeddingProvider> embeddingProvider_;
	std::shared_ptr<IVisionProvider> visionProvider_;
};