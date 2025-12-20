#pragma once
#include <memory>
#include "IModelProvider.h"

class IASRInferProvider;
class ThreadPool;

class IASRProvider final: public IModelProvider {
public:
	explicit IASRProvider(size_t n_thread = 1);
	virtual ~IASRProvider() = default;

	// Inherited via IModelProvider
	const char* name() const override;

	ModelCapability capability() const override;

	Status initialize(
		const char* model_path,
		const Model_Params& params) override;

	Status updateParams(
		const Model_Params& params) override;

	void unInitialize() override;

	Status transcribe(std::span<const float> samples, std::string &out);

private:
	std::unique_ptr<IASRInferProvider> engine_;
	std::unique_ptr<ThreadPool> pool_;
};