#pragma once
#include "IModelProvider.h"
#include "core_common.h"
#include "core_export.h"
#include "core_c_api.h"
#include <memory>
#include <mutex>

// Simple model hub that owns a concrete IModelProvider instance.
class AIModelHub {

	using ModelMap = std::unordered_map<std::string, std::shared_ptr<LocalAI_Model_t>>;
public:
	static AIModelHub& instance();

	Status_Internal initializeModel(const char *model_id, const char* model_path, const Model_Params& option);
	Status_Internal updateModelParams(const char* model_id, const Model_Params& option);

	LocalAI_ModelInfo_t getModelInfo(const char* model_id);
	bool getModelParams(const char *model_id, Model_Params &params);
	std::vector<std::string> supportedModels() const;

private:
	AIModelHub();
    std::mutex mtx_params_;
	std::unordered_map<std::string, std::shared_ptr<LocalAI_Model_t>> models_;
};