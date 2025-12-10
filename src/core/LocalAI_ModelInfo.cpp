#include "../core/core_c_api.h"
#include "core_common.h"
#include <string>

Model_Type LocalAI_GetModelType(LocalAI_ModelInfo* info) 
{
	if (info) {
		return info->type;
	}

	return LOCALAI_MODEL_UNKOWN;
}

const char* LocalAI_GetModelId(LocalAI_ModelInfo* info)
{
	if (info) {
		return info->id.c_str();
	}
	return nullptr;
}

const char* LocalAI_GetModelPath(LocalAI_ModelInfo* info)
{
	if (info) {
		return info->path.c_str();
	}
	return nullptr;
}

Model_Params* LocalAI_GetModelParams(LocalAI_ModelInfo* info)
{
	if (info) {
		return &info->params;
	}
	return nullptr;
}
