#include "AIModelHub.h"
#include "IModelProvider.h"

#include <format>

bool AIModelHub::addModel(Model_Type type, std::unique_ptr<IModelProvider> model)
{
    if (!model) {
        fprintf(stderr, "%s: Invalid model id or model provider.\n", __func__);
        return false;
    }

    auto it = models_.find(type);
    if (it != models_.end()) {
        fprintf(stderr, "%s: The model type: `%d` has already existed.\n", __func__, type);
        return false;
    }

    models_[type] = std::move(model);

    return true;
}

std::vector<Model_Type> AIModelHub::supportedModels() const
{
    std::vector<Model_Type> models;
    for (const auto &model : models_) {
        models.push_back(model.first);
    }
    return models;
}

Status AIModelHub::initializeModel(
    Model_Type type,
    const char* model_path, 
    const Model_Params& option)
{
    if (!model_path || *model_path == '\0') {
        return Status{
            LocalAI_ErrorCode::LOCALAI_INVALID_ARG,
            "Invalid model id or model path" 
        };
    }

    auto it = models_.find(type);
    if (it != models_.end()) {
        return it->second->initialize(model_path, option);
    }

    return { LocalAI_ErrorCode::LOCALAI_NO_MODEL, "Cannot find the specific model."};
}

Status AIModelHub::updateModelParams(Model_Type type, const Model_Params& option)
{
    std::lock_guard locker(mtx_params_);

    auto it = models_.find(type);
    if (it != models_.end()) {
        return it->second->updateParams(option);
    }

    return { LocalAI_ErrorCode::LOCALAI_NO_MODEL, "Cannot find the specific model." };
}

bool AIModelHub::getModelParams(Model_Type type, Model_Params& params)
{
    return false;
}

const char* AIModelHub::getModelName(Model_Type type)
{
    std::lock_guard locker(mtx_params_);
    auto it = models_.find(type);
    if (it != models_.end()) {
        return it->second->name();
    }

    return nullptr;
}
