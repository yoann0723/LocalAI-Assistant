#pragma once
#include "IModelProvider.h"

class IVisionInferProvider;
class ThreadPool;

class IVisionProvider final: public IModelProvider {
public:
	explicit IVisionProvider(size_t n_thread);
	virtual ~IVisionProvider() = default;

	// Inherited via IModelProvider
	const char* name() const override;

	ModelCapability capability() const override;

	Status initialize(
		const char* model_path, 
		const Model_Params& params) override;

	Status updateParams(
		const Model_Params& params) override;

	void unInitialize() override;

	Status visionInfer(
		const ImageView& image, std::string &out);

private:
	std::unique_ptr<IVisionInferProvider> engine_;
	std::unique_ptr<ThreadPool> pool_;
};