#include "ConversationManager.h"

ConversationManager::ConversationManager(size_t maxMessages, size_t maxTokens)
    : maxMessages_(maxMessages), maxTokens_(maxTokens) {}

void ConversationManager::addUserMessage(const std::string& msg) {
    history_.emplace_back(Message{Role::USER, msg});

    trimHistoryIfNeeded();
}

void ConversationManager::addAssistantMessage(const std::string& msg) {
    history_.emplace_back(Message{Role::ASSISTANT, msg});

    trimHistoryIfNeeded();
}

std::vector<Message> ConversationManager::buildHistoryForLLM() const {
    return std::vector<Message>(history_.begin(), history_.end());
}

std::string ConversationManager::buildRagQuery() const {
    // Simple concatenation of all user messages as RAG query
    std::string query;
    for (const auto& msg : history_) {
        if (msg.role == Role::USER) {
            if (!query.empty()) {
                query += " ";
            }
            query += msg.content;
        }
    }
    return query;
}

size_t ConversationManager::estimateTokens(const std::string& text) const {
    // Simple token estimation: 1 token per 4 characters
    return text.size() / 4;
}

void ConversationManager::trimHistoryIfNeeded() {
    // Trim by max messages
    while (history_.size() > maxMessages_) {
        history_.pop_front();
    }

    // Trim by max tokens
    size_t totalTokens = 0;
    for (const auto& msg : history_) {
        totalTokens += estimateTokens(msg.content);
    }

    while (totalTokens > maxTokens_ && !history_.empty()) {
        totalTokens -= estimateTokens(history_.front().content);
        history_.pop_front();
    }
}