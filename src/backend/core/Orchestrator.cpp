#include "Orchestrator.h"

Orchestrator::Orchestrator(
    ConversationManager* conv,
    RagRetriever* rag,
    LLMEngine* llm,
    PluginManager* pm
) : conv_(conv), rag_(rag), llm_(llm), pm_(pm)
{
}

void Orchestrator::handleUserInput(
    const std::string& userInput,
    LLMResponseCallbackFn callback)
{
    conv_->addUserMessage(userInput);

    // 1. RAG: qurey capabilities
    std::string ragQuery = conv_->buildRagQuery();
    auto candidates = rag_->retrieve(ragQuery);

    // 2. build LLM prompt (with multi-turn conversation and capabilities selection abilities)
    auto history = conv_->buildHistoryForLLM();
    llm_->infer(history, candidates, userInput,
        [callback, this](LLMResult result, Status status) {
        
        std::string response = "";
        if (!status) {
            callback(response, status);
            return;
        }

        if (result.isFunctionCall) {
			// call plugin capability
            response = pm_->execute(*result.fnCall);
        }
        else {
			// normal assistant response
            response = result.assistantMessage;
            conv_->addAssistantMessage(result.assistantMessage);
        }

        conv_->addAssistantMessage(response);
        callback(response, {});
    });
}
