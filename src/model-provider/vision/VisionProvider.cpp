#include "VisionProvider.h"
#include <assert.h>

ModelProvider::Result<std::string> VisionProvider::visionInfer(const ModelProvider::ImageView &image)
{
	if (image.data.empty()) {
		printf("Invalid image view data");
		return tl::unexpected(LocalAI_ErrorCode::LOCALAI_INVALID_ARG);
	}
	//TODO: Implement vision inference logic here
	return description_;
}
