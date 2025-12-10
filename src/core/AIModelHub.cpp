#include "AIModelHub.h"
#include "IModelProvider.h"

#include <format>

using namespace ModelProvider;

static Status_Internal initializeModelInternal(LocalAI_Model_t* model, const char* model_path, const Model_Params& option)
{
    if(!model)
        return { LocalAI_ErrorCode::LOCALAI_INVALID_ARG, "Model provider is null" };

    auto r = model->provider->initialize(model_path, option);
    if (!r) {
        std::string msg = std::format("Failed to initialize model. Model path: {}", model_path ? model_path : "");
        return { r.error(), msg };
    }
    else {
        model->info.path = std::string(model_path);
        model->info.params = option;
    }

    return {};
}

AIModelHub& AIModelHub::instance() {
	static AIModelHub hub;
	return hub;
}

AIModelHub::AIModelHub()
{
    /** Now we only support the following models. */
    models_[ID_LLM] = std::make_shared<LocalAI_Model_t>(ID_LLM, createLLMProvider);
    models_[ID_EMBEDDING_MODEL] = std::make_shared<LocalAI_Model_t>(ID_EMBEDDING_MODEL, createEmbeddingProvider);
    models_[ID_ASR_MODEL] = std::make_shared<LocalAI_Model_t>(ID_ASR_MODEL, createASRProvider);
    models_[ID_VISION_MODEL] = std::make_shared<LocalAI_Model_t>(ID_VISION_MODEL, createVisionProvider);
}

std::vector<std::string> AIModelHub::supportedModels() const
{
    std::vector<std::string> models;
    for (const auto &model : models_) {
        models.push_back(model.first);
    }
    return models;
}

Status_Internal AIModelHub::initializeModel(const char* model_id, const char* model_path, const Model_Params& option)
{
    if (!model_id || !model_path || *model_id == '\0' || *model_path == '\0') {
        return Status_Internal(LocalAI_ErrorCode::LOCALAI_INVALID_ARG, "Invalid model id or model path");
    }

    auto it = models_.find(model_id);
    if (it != models_.end()) {
        if (it->second->provider_factory && it->second->provider == nullptr) {
            it->second->provider = it->second->provider_factory();
            return ::initializeModelInternal(it->second.get(), model_path, option);
        }
    }

    return Status_Internal();
}

Status_Internal AIModelHub::updateModelParams(const char* model_id, const Model_Params& option)
{
    if (!model_id || *model_id == '\0') {
        return Status_Internal(LocalAI_ErrorCode::LOCALAI_INVALID_ARG, "Invalid model id");
    }

    std::lock_guard locker(mtx_params_);

    auto it = models_.find(model_id);
    if (it != models_.end()) {
        auto r = it->second->provider->updateParams(option);
        if (!r) {
            std::string msg = std::format("Failed to initialize model: {}", it->second->info.id);
            return { r.error(), msg };
        }
        else {
            it->second->info.params = option;
        }
    }
    else {
        return Status_Internal(LocalAI_ErrorCode::LOCALAI_NOT_INITIALIZED, "Model not initialized.");
    }
}

bool AIModelHub::getModelParams(const char* model_id, Model_Params& params)
{
    if (!model_id) {
        return false;
    }

    std::lock_guard locker(mtx_params_);
    auto it = models_.find(model_id);
    if (it != models_.end()) {
        params = it->second->info.params;
        return true;
    }

    return false;
}

LocalAI_ModelInfo_t AIModelHub::getModelInfo(const char* model_id)
{
    if (!model_id) {
        return {};
    }

    std::lock_guard locker(mtx_params_);
    auto it = models_.find(model_id);
    if (it != models_.end()) {
        return it->second->info;
    }

    return {};
}
