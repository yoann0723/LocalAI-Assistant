#include "ConversationManager.h"
constexpr const char* think_tag_start = "<think>";
constexpr const char* think_tag_end = "</think>";

void replace_all(std::string& s, const std::string& search, const std::string& replace) 
{
    if (search.empty()) {
        return;
    }
    std::string builder;
    builder.reserve(s.length());
    size_t pos = 0;
    size_t last_pos = 0;
    while ((pos = s.find(search, last_pos)) != std::string::npos) {
        builder.append(s, last_pos, pos - last_pos);
        builder.append(replace);
        last_pos = pos + search.length();
    }
    builder.append(s, last_pos, std::string::npos);
    s = std::move(builder);
}

ConversationManager::ConversationManager(size_t maxMessages, size_t maxTokens)
    : maxMessages_(maxMessages), maxTokens_(maxTokens) {}

void ConversationManager::addUserMessage(const std::string& msg) 
{
    history_.emplace_back(Message{Role::USER, msg});

    trimHistoryIfNeeded();
}

void ConversationManager::addAssistantMessage(const std::string& msg) 
{
    bool is_think = msg.starts_with(think_tag_start);
    auto id_end = msg.find(think_tag_end);
    if (is_think && id_end > 0) {
        auto sub = msg.substr(id_end + strlen(think_tag_end));
        replace_all(sub, "\n", "");
        history_.emplace_back(Message{Role::ASSISTANT, sub});
    }

    trimHistoryIfNeeded();
}

std::vector<Message> ConversationManager::buildHistoryForLLM() const 
{
    return std::vector<Message>(history_.begin(), history_.end());
}

std::string ConversationManager::buildRagQuery() const 
{
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

size_t ConversationManager::estimateTokens(const std::string& text) const 
{
    // Simple token estimation: 1 token per 4 characters
    return text.size() / 4;
}

void ConversationManager::trimHistoryIfNeeded() 
{
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