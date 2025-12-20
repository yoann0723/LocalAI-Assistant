#pragma once
#include <string>
#include <vector>
#include <optional>
#include "ConversationManager.h"
#include "RagRetriever.h"
#include "AIModelHub.h"

struct FunctionCall {
    std::string name;
    std::string argumentsJSON;
};

struct LLMResult {
    bool isFunctionCall = false;
    std::string assistantMessage;
    std::optional<FunctionCall> fnCall;
};

using LLMInferCallbackFn = std::function<void(LLMResult result, Status status)>;

class LLMEngine {
public:
    LLMEngine(AIModelHub *hub);

    void infer(
        const std::vector<Message>& history,
        const std::vector<CapabilityInfo>& candidates,
        const std::string& userInput,
        LLMInferCallbackFn callback
    );

private:
    const AIModelHub* model_hub_ = nullptr;
};
