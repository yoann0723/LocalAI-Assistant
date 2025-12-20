#pragma once
#include <onnxruntime_cxx_api.h>
#include "IInferenceProvider.h"

class OrtInferenceSession final : public ITensorInferProvider {

public:
	OrtInferenceSession() = default;
	~OrtInferenceSession() override;

	// Inherited via IInferenceProvider
	Status initialize(
		const char* model_path, 
		const Model_Params& params) override;

	Status updateParams(
		const Model_Params& params) override;

	Status run(
		const InferenceRequest& request,
		Variant *result) override;

	Status runAsync(
		const InferenceRequest& request, 
		InferenceCallback callback, 
		void* user_data) override;

private:
	std::shared_ptr<Ort::Session> session_;
};
