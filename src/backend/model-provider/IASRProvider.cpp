#include "IASRProvider.h"
#include "IInferenceProvider.h"
#include "ThreadPool.h"

ASRProviderImpl::ASRProviderImpl(size_t n_thread)
	:engine_(std::move(InferenceFactory::createASRInfer())), 
	pool_(std::make_unique<ThreadPool>(n_thread == 0 ? 1 : n_thread)) {
}

const char* ASRProviderImpl::name() const
{
	return "asr_model";
}

ModelCapability ASRProviderImpl::capability() const
{
	return ModelCapability::ASR;
}

Status ASRProviderImpl::initialize(const char* model_path, const Model_Params& params)
{
	if(engine_)
		return engine_->initialize(model_path, params);

	return { LOCALAI_NOT_INITIALIZED, "Inference engine is not intialized." };
}

Status ASRProviderImpl::updateParams(const Model_Params& params)
{
	return {};
}

void ASRProviderImpl::unInitialize()
{
}

Status ASRProviderImpl::transcribe(std::span<const float> samples, std::string &out)
{
	if (!engine_) {
		assert(false);
		return { LOCALAI_NOT_INITIALIZED, "Inference engine is not intialized." };
	}

	if (samples.empty()) {
		printf("Invalid audio sample input for ASR.\n");
		return {
			LocalAI_ErrorCode::LOCALAI_INVALID_ARG,
			"Invalid audio sample input for ASR"
		};
	}

	return engine_->transcribe(samples, out);
}