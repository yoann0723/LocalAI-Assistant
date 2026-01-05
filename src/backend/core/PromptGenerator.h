#include <string>
#include <vector>
#include <sstream>
#include <json.hpp>
#include "../common/common.hpp"

class PromptGenerator {
public:
	static std::vector<Message> buildPrompt(
		const std::vector<Message>& conversation_history, // pair<role, content>
		const std::string& user_input,
		const std::vector<CapabilityInfo>& rag_capabilities
	);

    static nlohmann::json messagesToJson(const std::vector<Message>& messages);

	static std::string promptApplyTemplate(const std::vector<Message>& messages, bool add_ass);
};