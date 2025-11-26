#pragma once
#include "IModelProvider.h"
#include <future>

class VisionProvider : public IVisionProvider {
	public:
	virtual ~VisionProvider() = default;
	// Asynchronous call returning future
	virtual std::future<ImageInfo> visionInfer(const ImageData& img) override {
		return std::async(std::launch::async, []() {
			// Placeholder implementation
			ImageInfo info;
			info.description = "This is a placeholder image description.";
			return info;
		});
	}
};