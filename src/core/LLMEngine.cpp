#include "LLMEngine.h"
#include "AIModelHub.h"
#include <nlohmann/json.hpp>

LLMEngine::LLMEngine(std::string apiKey)
    : apiKey_(std::move(apiKey)) {}

LLMResult LLMEngine::infer(
    const std::vector<Message>& history,
    const std::vector<CapabilityInfo>& candidates,
    const std::string& userInput
) {
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
    auto llmProvider = AIModelHub::instance().textModelProvider();
    auto futureResult = llmProvider->generateLLM(prompt);
    auto inferenceResult = futureResult.get();
    LLMResult result;
    if (!inferenceResult.ok) {
        result.assistantMessage = "Error during LLM inference.";
        return result;
    }

    // Simple parsing to check for function call in the response
    std::string response = inferenceResult.text;
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
            return result;
        }
    }

    result.assistantMessage = response;
    return result;
}