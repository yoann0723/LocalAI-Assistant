#include "IVisionProvider.h"
#include "IInferenceProvider.h"
#include "ThreadPool.h"
#include <assert.h>

IVisionProvider::IVisionProvider(size_t n_thread)
	:engine_(std::move(InferenceFactory::createVisionInfer())),
	pool_(std::make_unique<ThreadPool>(n_thread == 0 ? 2 : n_thread))
{
}

const char* IVisionProvider::name() const
{
	return "vision";
}

ModelCapability IVisionProvider::capability() const
{
	return ModelCapability::Vision;
}

Status IVisionProvider::initialize(const char* model_path, const Model_Params& params)
{
	return {};
}

Status IVisionProvider::updateParams(const Model_Params& params)
{
	return {};
}

void IVisionProvider::unInitialize()
{
}

Status IVisionProvider::visionInfer(const ImageView& image, std::string& out)
{
	if (image.data.empty()) {
		printf("Invalid image view data");
		return { 
			LocalAI_ErrorCode::LOCALAI_INVALID_ARG,
			"Invalid image view data"
		};
	}
	//TODO: Implement vision inference logic here
	return {};
}