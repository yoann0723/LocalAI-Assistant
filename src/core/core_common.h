#pragma once

#include "../core/core_c_api.h"
#include "IModelProvider.h"

#include <string>
#include <memory>

#if defined(_WIN32)
#define LAACHAR_T wchar_t
#else
#define LAACHAR_T char
#endif

constexpr const char* ID_LLM = "localai_llm";
constexpr const char* ID_EMBEDDING_MODEL = "localai_embedding_model";
constexpr const char* ID_ASR_MODEL = "localai_asr_model";
constexpr const char* ID_VISION_MODEL = "localai_vision_model";

constexpr const int DEFAULT_N_LLM_THREAD = 1;
constexpr const int DEFAULT_N_EMBEDDING_THREAD = 2;
constexpr const int DEFAULT_N_VISION_THREAD = 2;

struct Status_Internal {
	LocalAI_ErrorCode code = LocalAI_ErrorCode::LOCALAI_OK;
	std::string msg = std::string("");
};

struct LocalAI_ModelInfo_t {
	std::string id = std::string("");
	std::string path = std::string("");
	Model_Params params = {};
	Model_Type type = LOCALAI_MODEL_UNKOWN;
};

struct LocalAI_Model_t {
	using Provider_Factory_p = std::unique_ptr<ModelProvider::ModelProviderBase>(*)();
	LocalAI_Model_t(const char* _id, Provider_Factory_p pf) :id(_id ? _id : ""), provider_factory(pf){}

	const char* id;
	std::unique_ptr<ModelProvider::ModelProviderBase> provider;
	Provider_Factory_p provider_factory = nullptr;
	LocalAI_ModelInfo_t info;
};


