#pragma once
#include "ConversationManager.h"
#include "RagRetriever.h"
#include "LLMEngine.h"
#include "PluginManager.h"

using LLMResponseCallbackFn = std::function<void(const std::string& response, Status status)>;

class Orchestrator {
public:
    Orchestrator(
        ConversationManager* conv,
        RagRetriever* rag,
        LLMEngine* llm,
        PluginManager* pm
    );

    void handleUserInput(
        const std::string& userInput, 
        LLMResponseCallbackFn callback);

private:
    ConversationManager* conv_;
    RagRetriever* rag_;
    LLMEngine* llm_;
    PluginManager* pm_;
};
