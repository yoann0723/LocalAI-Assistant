#include "ICustomModelProvider.h"
#include "IInferenceProvider.h"
#include "ThreadPool.h"

ICustomModelProvider::ICustomModelProvider(size_t n_thread)
	:engine_(std::move(InferenceFactory::createTensorInfer())),
	pool_(std::make_unique<ThreadPool>(n_thread))
{
}

const char* ICustomModelProvider::name() const
{
	return "custom model";
}

ModelCapability ICustomModelProvider::capability() const
{
	return ModelCapability();
}

Status ICustomModelProvider::initialize(const char* model_path, const Model_Params& params)
{
	return {};
}

Status ICustomModelProvider::updateParams(const Model_Params& params)
{
	return {};
}

void ICustomModelProvider::unInitialize()
{
}

void ICustomModelProvider::setShapes(const std::vector<int64_t>& input, const std::vector<int64_t>& output)
{
}

void ICustomModelProvider::setInputTensor(const TensorData& input)
{
}

void ICustomModelProvider::getOutputTensor(TensorData& output)
{
}
