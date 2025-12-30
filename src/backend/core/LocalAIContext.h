#pragma once
#include "localai_c_api.h"
#include "core_common.h"
#include "../common/common.hpp"

#include "AIModelHub.h"
#include "LLMEngine.h"
#include "Orchestrator.h"
#include "IEmbeddingProvider.h"
#include "ILLMProvider.h"
#include "IVisionProvider.h"
#include "IASRProvider.h"
#include "Pipeline.h"

struct LocalAI_Context_t;
struct ChatSessionImpl;

struct LocalAI_Request_t {
	struct Impl;
	std::unique_ptr<Impl> impl_;

	LocalAI_Request_t();
	~LocalAI_Request_t();
	LocalAI_RequestStatus status();
	void cancel();
};

struct LocalAI_ChatSession_t {

	void chat(std::string_view message, std::string_view params, LocalAI_Request** out_request,
		LocalAI_TextCallback callback, void *user_data);

	/*std::unique_ptr<AsyncPipeline> chat_pipeline_;

	std::unique_ptr<ConversationManager> conv_;
	std::unique_ptr<LLMEngine> llm_engine_;
	std::unique_ptr<Orchestrator> orch_;
	std::unique_ptr<RagRetriever> rag_;

	std::weak_ptr<AIModelHub> model_hub_;
	std::weak_ptr<PluginManager> pm_;*/

	std::shared_ptr<ChatSessionImpl> impl_;
};

using ChatSessionSharedPtr = std::shared_ptr<ChatSessionImpl>;
using ChatSessionWeakPtr = std::weak_ptr<ChatSessionImpl>;

struct LocalAI_Context_t {
	
	explicit LocalAI_Context_t(const LocalAI_Config& config);
	~LocalAI_Context_t();

	const IEmbeddingProvider* embeddingProvider() {
		return model_hub_->modelProvider<IEmbeddingProvider>(Model_Type::LOCALAI_MODEL_EMBEDDING);
	}

	const ILLMProvider* llmProvider() {
		return model_hub_->modelProvider<ILLMProvider>(Model_Type::LOCALAI_MODEL_TEXT_GEN);
	}

	const IVisionProvider* visionProvider() {
		return model_hub_->modelProvider<IVisionProvider>(Model_Type::LOCALAI_MODEL_VISION);
	}

	const IASRProvider* asrProvider() {
		return model_hub_->modelProvider<IASRProvider>(Model_Type::LOCALAI_MODEL_ASR);
	}

	ChatSessionSharedPtr createChatSession();

	void removeChatSession(const ChatSessionSharedPtr& session);

	LocalAI_Config config_;
	std::mutex session_mtx_;
	std::vector<ChatSessionWeakPtr> chat_sessions_;
	std::unique_ptr<AIModelHub> model_hub_;
	std::unique_ptr<PluginManager> pm_;
};
