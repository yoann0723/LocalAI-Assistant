#pragma once
#include "ConversationManager.h"
#include "RagRetriever.h"
#include "LLMEngine.h"
#include "PluginManager.h"

class Orchestrator {
public:
    Orchestrator(
        ConversationManager* conv,
        RagRetriever* rag,
        LLMEngine* llm,
        PluginManager* pm
    );

    std::string handleUserInput(const std::string& userInput);

private:
    ConversationManager* conv_;
    RagRetriever* rag_;
    LLMEngine* llm_;
    PluginManager* pm_;
};
