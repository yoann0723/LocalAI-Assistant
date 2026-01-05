#pragma once
#include <string>
#include <deque>
#include <vector>
#include "../common/common.hpp"

//enum class Role {
//    USER,
//    ASSISTANT,
//    SYSTEM
//};
//
//struct Message {
//    Role role;
//    std::string content;
//};

class ConversationManager {
public:
    ConversationManager(size_t maxMessages = 10, size_t maxTokens = 1024);

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
