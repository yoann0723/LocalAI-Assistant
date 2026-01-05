#include "ILLMProvider.h"
#include "IInferenceProvider.h"
#include "ThreadPool.h"
#include "../common/common.hpp"

LLMProviderImpl::LLMProviderImpl(size_t n_thread)
	:engine_(std::move(InferenceFactory::createLLMInfer())),
	pool_(std::make_unique<ThreadPool>(n_thread == 0 ? 1 : n_thread).release())
{
}

LLMProviderImpl::~LLMProviderImpl() 
{
	unInitialize();
}

const char* LLMProviderImpl::name() const
{
	return "llm";
}

ModelCapability LLMProviderImpl::capability() const
{
	return ModelCapability::TextGeneration;
}

Status LLMProviderImpl::initialize(const char* model_path, const Model_Params& params)
{
	return engine_->initialize(model_path, params);
}

Status LLMProviderImpl::updateParams(const Model_Params& params)
{
	return engine_->updateParams(params);
}

void LLMProviderImpl::unInitialize()
{
	pool_->stop();
}

//void LLMProviderImpl::generateAsync(
//	std::string_view prompt, std::optional<std::string_view> params,
//	size_t max_len, 
//	LocalAI_TextCallback callback, void* user_data) const
//{
//
//	if (prompt.empty()) 
//	{
//		printf("Empty text input for text generation.\n");
//	}
//	
//
//	pool_->run([temp_prompt = std::move(std::string(prompt)), callback, this, user_data]() {
//		LocalAI_TextResult_t *response = nullptr;
//		auto status = engine_->generate(temp_prompt, &response);
//		if (callback) 
//			callback(nullptr, response, LocalAI::StatusConvert(status), user_data);
//	});
//}

void LLMProviderImpl::generateAsync(
	std::string_view prompt, std::optional<std::string_view> params,
	size_t max_len, LLMCallback callback) const
{
	if (prompt.empty())
	{
		fprintf(stderr, "Empty text input for text generation.\n");
	}


	pool_->run([temp_prompt = std::move(std::string(prompt)), callback, this]() {
		LLMOutput *response = nullptr;
		auto status = engine_->generate(temp_prompt, &response);
		if (callback)
			callback(response, status);
	});
}
