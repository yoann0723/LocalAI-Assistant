#pragma once
#include <string>
#include <variant>
#include <functional>
#include <span>
#include <format>
#include "localai_c_api.h"

struct Status {
    LocalAI_ErrorCode code = LocalAI_ErrorCode::LOCALAI_OK;
	std::string error;

    operator bool() const {
        return code == LocalAI_ErrorCode::LOCALAI_OK;
    }

    // Combine error code and msg. Used for logs
    std::string str() const{
        return std::format("error: {:d}, msg: {}", static_cast<int>(code), error);
    }
};

typedef enum DataType {
    DATA_TYPE_NONE = 0,
    DATA_TYPE_INT,
    DATA_TYPE_INTS,
    DATA_TYPE_FLOAT,
    DATA_TYPE_FLOATS,
    DATA_TYPE_STRING,
    DATA_TYPE_STRINGS
}DataType;

typedef struct Variant {
    DataType type;
    union {
        uint32_t i32;
        uint32_t* i32s;
        float f32;
        float* f32s;
        char* str;
    };
}Variant;

enum class InferenceType {
    Inference_Tensor,
    Inference_LLM,
    Inference_ASR,
};

struct ImageView {
    std::span<const uint8_t> data;
    int width;
    int height;
    int channels;
    std::string_view format;
};

struct LLMOutput {
    std::string text;
    int piece_count = 0;
};

struct EmbeddingOutput {
    std::vector<float> embedding;
};

struct RagOutput {
    std::vector<std::string> top_k;
};

struct PluginOutput {

};

enum class StepStatus {
    Ok,
    Failed,
    Cancelled
};

enum class PipelineStatus {
    Idle,
    Running,
    Stopped
};

struct StepResult {
    StepStatus status;
    std::string message;
};

enum class Role:int8_t {
    USER,
    ASSISTANT,
    SYSTEM
};

struct Message {
    Role role;
    std::string content;
};

struct CapabilityInfo {
    std::string name;
    std::string description;
    std::vector<float> embedding;
};

namespace LocalAI {
    inline LocalAI_Status* StatusConvert(Status status) {
        if (status) {
            return nullptr;
        }
        return LocalAI_CreateStatus(status.code, status.error.c_str());
    }
}

