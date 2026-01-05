#include "PromptGenerator.h"

constexpr const int default_max_history = 10;

static constexpr const char *RoleToString(Role role)
{
    if (Role::USER == role)
        return "user";

    if (Role::SYSTEM == role)
        return "system";

    if (Role::ASSISTANT == role)
        return "assistant";

    return "unknown";
}

const std::string SYSTEM_PROMPT = R"(
You are an AI assistant that supports multi-turn conversation and tool usage.
Rules:
1. Use conversation history to understand user intent.
2. Only call functions from the provided capabilities.
3. Only call a function if all required parameters are available.
4. If information is missing or ambiguous, ask a clarifying question.
5. Output valid JSON ONLY when calling a function:
{
  "name": "function_name",
  "arguments": { ...parameters... }
}
6. Do not invent functions or parameters.
7. Only call one function per response.
8. If no function matches, reply in natural language.
)";

std::vector<Message> PromptGenerator::buildPrompt(
    const std::vector<Message>& conversation_history,
    const std::string& user_input,
    const std::vector<CapabilityInfo>& rag_capabilities
) {
    std::vector<Message> messages;
    // System prompt
    messages.push_back(Message{ Role::SYSTEM, SYSTEM_PROMPT});

    // Conversation history
    size_t max_history = default_max_history;
    size_t start_idx = conversation_history.size() > max_history ? 
                       conversation_history.size() - max_history : 0;
    for (size_t i = start_idx; i < conversation_history.size(); ++i) {
        messages.push_back(conversation_history[i]);
    }

    // RAG capabilities as system message
    if (!rag_capabilities.empty()) {
        std::ostringstream oss;
        oss << "Available capabilities:\n";
        for (const auto& cap : rag_capabilities) {
            oss << "- " << cap.description << "\n";
        }
        messages.push_back(Message{Role::SYSTEM, oss.str()});
    }

    // User input
    messages.push_back(Message{ Role::USER, user_input});

    return messages;
}

nlohmann::json PromptGenerator::messagesToJson(const std::vector<Message>& messages) 
{
	nlohmann::json jMessages = nlohmann::json::array();
	for (const auto& msg : messages) {
		nlohmann::json jMsg;
		jMsg["role"] = RoleToString(msg.role);
		jMsg["content"] = msg.content;
		jMessages.push_back(jMsg);
	}
	return jMessages;
}

std::string PromptGenerator::promptApplyTemplate(const std::vector<Message>& messages, bool add_ass)
{
    std::stringstream ss;
    for (const auto& msg : messages) {
        ss << "<|im_start|>" << RoleToString(msg.role) << "\n" << msg.content << "<|im_end|>\n";
    }

    if (add_ass) {
        ss << "<|im_start|>assistant\n";
    }

    return ss.str();
}
