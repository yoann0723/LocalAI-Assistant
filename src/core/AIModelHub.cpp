#include "AIModelHub.h"

AIModelHub& AIModelHub::instance() {
    static AIModelHub hub;
    return hub;
}

void AIModelHub::setTextModelProvider(std::shared_ptr<ILLMProvider> provider) {
    std::lock_guard<std::mutex> lg(mtx_);
    textProvider_ = std::move(provider);
}

std::shared_ptr<ILLMProvider> AIModelHub::textModelProvider() {
    std::lock_guard<std::mutex> lg(mtx_);
    return textProvider_;
}

void AIModelHub::setEmbeddingModelProvider(std::shared_ptr<IEmbeddingProvider> provider)
{
	std::lock_guard<std::mutex> lg(mtx_);
	embeddingProvider_ = std::move(provider);
}

std::shared_ptr<IEmbeddingProvider> AIModelHub::embeddingModelProvider()
{
	return embeddingProvider_;
}

void AIModelHub::setVisionModelProvider(std::shared_ptr<IVisionProvider> provider)
{
    std::lock_guard<std::mutex> lg(mtx_);
	visionProvider_ = std::move(provider);
}

std::shared_ptr<IVisionProvider> AIModelHub::visionModelProvider()
{
	return visionProvider_;
}
