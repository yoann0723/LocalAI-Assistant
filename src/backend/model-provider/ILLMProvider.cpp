#include "ILLMProvider.h"
#include "IInferenceProvider.h"
#include "ThreadPool.h"

ILLMProvider::ILLMProvider(size_t n_thread)
	:engine_(std::move(InferenceFactory::createLLMInfer())),
	pool_(std::make_unique<ThreadPool>(n_thread == 0 ? 1 : n_thread).release())
{
}

ILLMProvider::~ILLMProvider() 
{
	unInitialize();
}

const char* ILLMProvider::name() const
{
	return "llm";
}

ModelCapability ILLMProvider::capability() const
{
	return ModelCapability::TextGeneration;
}

Status ILLMProvider::initialize(const char* model_path, const Model_Params& params)
{
	return engine_->initialize(model_path, params);
}

Status ILLMProvider::updateParams(const Model_Params& params)
{
	return engine_->updateParams(params);
}

void ILLMProvider::unInitialize()
{
	pool_->stop();
}

void ILLMProvider::generateAsync(
	std::string_view prompt,
	size_t max_len, 
	LLMCallback callback) const
{

	if (prompt.empty()) 
	{
		printf("Invalid text input for embedding.\n");
		if (callback) {
			callback("", {
				LocalAI_ErrorCode::LOCALAI_INVALID_ARG,
				"Invalid text input for embedding"
				});
		}
	}
	else {
		pool_->run([temp_prompt = std::move(std::string(prompt)), callback, this]() {
			std::string response;
			auto status = engine_->generate(temp_prompt, response);
			if (callback) callback(response, status);
		});
	}
}