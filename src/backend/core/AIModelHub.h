#pragma once
#include "IModelProvider.h"
#include "core_common.h"
#include "core_export.h"
#include "localai_c_api.h"
#include <memory>
#include <mutex>

// Simple model hub that owns a concrete IModelProvider instance.
class AIModelHub {

	using ModelProviderMap = std::unordered_map<Model_Type, std::unique_ptr<IModelProvider>>;
public:
	AIModelHub() = default;
	~AIModelHub() = default;

	bool addModel(Model_Type type, std::unique_ptr<IModelProvider> model);

	std::vector<Model_Type> supportedModels() const;

	const char* getModelName(Model_Type type);

	template <class T>
	const T* modelProvider(Model_Type type) const {

		auto it = models_.find(type);
		if (it != models_.end()) {
			return dynamic_cast<T*>(it->second.get());
		}

		return nullptr;
	}

	bool getModelParams(
		Model_Type type,
		Model_Params& params);

	Status initializeModel(
		Model_Type type, 
		const char* model_path, 
		const Model_Params& option);

	Status updateModelParams(
		Model_Type type,
		const Model_Params& option);

private:
    std::mutex mtx_params_;
	ModelProviderMap models_;
};