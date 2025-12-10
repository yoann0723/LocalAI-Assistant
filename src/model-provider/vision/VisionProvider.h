#pragma once
#include "IModelProvider.h"
#include <future>

class VisionProvider : public ModelProvider::IVisionProvider {
public:
	virtual ~VisionProvider() = default;
	
	virtual ModelProvider::Result<std::string> visionInfer(const ModelProvider::ImageView &image) override;
private:
	std::string description_;
};