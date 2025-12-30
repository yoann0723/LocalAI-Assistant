#include "LLMEngine.h"
#include "AIModelHub.h"
#include "ILLMProvider.h"
#include <json.hpp>

LLMEngine::LLMEngine(AIModelHub* hub)
    :model_hub_(hub)
{}

void LLMEngine::infer(
    const std::vector<Message>& history,
    const std::vector<CapabilityInfo>& candidates,
    const std::string& userInput,
    LLMInferCallbackFn callback
) {
#if 0
    // Build prompt from history, candidates, and user input
    std::string prompt;
    for (const auto& msg : history) {
        if (msg.role == Role::USER) {
            prompt += "User: " + msg.content + "\n";
        } else if (msg.role == Role::ASSISTANT) {
            prompt += "Assistant: " + msg.content + "\n";
        }
    }
    prompt += "Available capabilities:\n";
    for (const auto& cap : candidates) {
        prompt += "- " + cap.name + ": " + cap.description + "\n";
    }   
    prompt += "User: " + userInput + "\nAssistant:";

    // Call LLM model via AIModelHub
    auto llmProvider = model_hub_->modelProvider<ILLMProvider>(LOCALAI_MODEL_TEXT_GEN);
    llmProvider->generateAsync(prompt, DEFAULT_LLM_MAX_CONTENT_LEN, 
        [callback](const std::string& response, const Status& status) {
            LLMResult result;
            if (!status) {
                result.assistantMessage = "Error during LLM inference.";
                callback(result, status);
                return;
            }

            // Simple parsing to check for function call in the response
            if (response.find("CALL_FUNCTION:") != std::string::npos) {
                size_t start = response.find("CALL_FUNCTION:") + 14;
                size_t end = response.find("\n", start);
                std::string fnCallStr = response.substr(start, end - start);
                size_t sep = fnCallStr.find("|");
                if (sep != std::string::npos) {
                    FunctionCall fnCall;
                    fnCall.name = fnCallStr.substr(0, sep);
                    fnCall.argumentsJSON = fnCallStr.substr(sep + 1);
                    result.isFunctionCall = true;
                    result.fnCall = fnCall;
                    callback(result, status);
                    return;
                }
            }

            result.assistantMessage = response;
            callback(result, status);
        }, this);
#endif
}