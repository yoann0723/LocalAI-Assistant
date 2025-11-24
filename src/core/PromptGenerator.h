#include <string>
#include <vector>
#include <sstream>
#include <nlohmann/json.hpp>

struct Message {
    std::string role;    // "system" / "user" / "assistant"
    std::string content;
};

class PromptGenerator {
public:
    static std::vector<Message> buildPrompt(
    const std::vector<std::pair<std::string,std::string>>& conversation_history, // pair<role, content>
    const std::string& user_input,
    const std::vector<std::string>& rag_capabilities
    );

    static nlohmann::json messagesToJson(const std::vector<Message>& messages);
};