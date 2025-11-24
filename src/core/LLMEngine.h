#pragma once
#include <string>
#include <vector>
#include <optional>
#include "ConversationManager.h"
#include "RagRetriever.h"

struct FunctionCall {
    std::string name;
    std::string argumentsJSON;
};

struct LLMResult {
    bool isFunctionCall = false;
    std::string assistantMessage;
    std::optional<FunctionCall> fnCall;
};

class LLMEngine {
public:
    LLMEngine(std::string apiKey);

    LLMResult infer(
        const std::vector<Message>& history,
        const std::vector<CapabilityInfo>& candidates,
        const std::string& userInput
    );

private:
    std::string apiKey_;
};
