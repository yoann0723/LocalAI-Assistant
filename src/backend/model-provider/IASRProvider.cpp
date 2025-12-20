#include "IASRProvider.h"
#include "IInferenceProvider.h"
#include "ThreadPool.h"

IASRProvider::IASRProvider(size_t n_thread)
	:engine_(std::move(InferenceFactory::createASRInfer())), 
	pool_(std::make_unique<ThreadPool>(n_thread == 0 ? 1 : n_thread)) {
}

const char* IASRProvider::name() const
{
	return "asr_model";
}

ModelCapability IASRProvider::capability() const
{
	return ModelCapability::ASR;
}

Status IASRProvider::initialize(const char* model_path, const Model_Params& params)
{
	return {};
}

Status IASRProvider::updateParams(const Model_Params& params)
{
	return {};
}

void IASRProvider::unInitialize()
{
}

Status IASRProvider::transcribe(std::span<const float> samples, std::string &out)
{
	if (samples.empty()) {
		printf("Invalid audio sample input for ASR.\n");
		return {
			LocalAI_ErrorCode::LOCALAI_INVALID_ARG,
			"Invalid audio sample input for ASR"
		};
	}

	return {};
}