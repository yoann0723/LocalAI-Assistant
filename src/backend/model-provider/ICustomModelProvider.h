#pragma once
#include "IModelProvider.h"

struct TensorData {
	std::vector<float> data;
};

class ITensorInferProvider;
class ThreadPool;

class ICustomModelProvider final : public IModelProvider {
public:
	explicit ICustomModelProvider(size_t n_thread = 1);
	virtual ~ICustomModelProvider() = default;

	// Inherited via IModelProvider
	const char* name() const override;

	ModelCapability capability() const override;

	Status initialize(
		const char* model_path,
		const Model_Params& params) override;

	Status updateParams(
		const Model_Params& params) override;

	void unInitialize() override;

	void setShapes(
		const std::vector<int64_t>& input,
		const std::vector<int64_t>& output);

	void setInputTensor(
		const TensorData& input);

	void getOutputTensor(
		TensorData& output);

	void run();

private:
	std::unique_ptr<ITensorInferProvider> engine_;
	std::unique_ptr<ThreadPool> pool_;
};
