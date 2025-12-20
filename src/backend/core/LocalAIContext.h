#pragma once
#include "../core/localai_c_api.h"
#include "../core/core_common.h"
#include "../common/common.h"

#include "AIModelHub.h"
#include "LLMEngine.h"
#include "Orchestrator.h"
#include "IEmbeddingProvider.h"
#include "ILLMProvider.h"
#include "IVisionProvider.h"
#include "IASRProvider.h"


struct LocalAI_Context_t {
	explicit LocalAI_Context_t(const LocalAI_Config& config);

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

	~LocalAI_Context_t() {}

	LocalAI_Config config_;
	std::unique_ptr<AIModelHub> model_hub_;
	std::unique_ptr<LLMEngine> llm_engine_;
	std::unique_ptr<ConversationManager> conv_;
	std::unique_ptr<RagRetriever> rag_;
	std::unique_ptr<PluginManager> pm_;
	std::unique_ptr<Orchestrator> orch_;
};