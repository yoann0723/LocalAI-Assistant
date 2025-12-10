#include "InferenceEngine.h"
#include "IModelProvider.h"

constexpr const size_t max_token_size = 768;

InferenceEngine::InferenceEngine(int llm_n_threads, int emb_n_threads, int vision_n_threads)
	:llm_pool_(llm_n_threads),
	emb_pool_(emb_n_threads),
	vision_pool_(vision_n_threads)
{
}

InferenceEngine::~InferenceEngine() {
	stop();
}

std::future<InferenceResult<std::string>> InferenceEngine::submitLLM(const std::string& prompt)
{
	llm_pool_.submit([prompt](std::string_view prompt) {
		
		}, prompt);
	return std::future<InferenceResult<std::string>>();
}

std::future<InferenceResult<std::string>> InferenceEngine::submitASR(std::span<const float> samples)
{
	return std::future<InferenceResult<std::string>>();
}

std::future<InferenceResult<std::span<const float>>> InferenceEngine::submitEmbedding(const std::string& text)
{
	return std::future<InferenceResult<std::span<const float>>>();
}

std::future<InferenceResult<std::string>> InferenceEngine::submitVision(const ModelProvider::ImageView& image)
{
	return std::future<InferenceResult<std::string>>();
}

void InferenceEngine::start() {
	
}

void InferenceEngine::stop() {
	llm_pool_.stop();
	emb_pool_.stop();
	vision_pool_.stop();
}