#pragma once
#include <string>
#include <deque>
#include <vector>

enum class Role {
    USER,
    ASSISTANT,
    SYSTEM
};

struct Message {
    Role role;
    std::string content;
};

class ConversationManager {
public:
    ConversationManager(size_t maxMessages, size_t maxTokens);

    void addUserMessage(const std::string& msg);
    void addAssistantMessage(const std::string& msg);

    std::vector<Message> buildHistoryForLLM() const;
    std::string buildRagQuery() const;

private:
    size_t estimateTokens(const std::string& text) const;
    void trimHistoryIfNeeded();

private:
    std::deque<Message> history_;
    size_t maxMessages_;
    size_t maxTokens_;
};
