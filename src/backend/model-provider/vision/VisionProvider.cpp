#include "IVisionProvider.h"
#include <assert.h>

const char* IVisionProvider::name() const
{
	return nullptr;
}

ModelCapability IVisionProvider::capability() const
{
	return ModelCapability();
}

Result<void> IVisionProvider::initialize(const char* model_path, const Model_Params& params)
{
	return Result<void>();
}

Result<void> IVisionProvider::updateParams(const Model_Params& params)
{
	return Result<void>();
}

void IVisionProvider::unInitialize()
{
}

Result<std::string> IVisionProvider::visionInfer(const ModelProvider::ImageView& image)
{
	if (image.data.empty()) {
		printf("Invalid image view data");
		return tl::unexpected(LocalAI_ErrorCode::LOCALAI_INVALID_ARG);
	}
	//TODO: Implement vision inference logic here
	return description_;
}