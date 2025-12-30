#pragma once

#include "localai_c_api.h"

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

constexpr const int DEFAULT_LLM_MAX_CONTENT_LEN = 2;


