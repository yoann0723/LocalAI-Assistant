#include "Orchestrator.h"

Orchestrator::Orchestrator(
    ConversationManager* conv,
    RagRetriever* rag,
    LLMEngine* llm,
    PluginManager* pm
) : conv_(conv), rag_(rag), llm_(llm), pm_(pm)
{
}

std::string Orchestrator::handleUserInput(const std::string& userInput)
{
    conv_->addUserMessage(userInput);

    // 1. RAG：检索当前语境可能关联的 capabilities
    std::string ragQuery = conv_->buildRagQuery();
    auto candidates = rag_->retrieve(ragQuery);

    // 2. 构造 LLM 输入（带多轮对话、候选能力）
    auto history = conv_->buildHistoryForLLM();
    auto result = llm_->infer(history, candidates, userInput);

    if (result.isFunctionCall) {
        // 3. 调用插件
        auto response = pm_->execute(*result.fnCall);

        conv_->addAssistantMessage(response);
        return response;
    }

    // 普通文本回复
    conv_->addAssistantMessage(result.assistantMessage);
    return result.assistantMessage;
}
