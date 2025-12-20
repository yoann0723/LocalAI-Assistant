#include "LocalAIContext.h"

 static std::unique_ptr<LocalAI_Context_t> g_context;
 static std::once_flag g_context_flag;

 static BuildInModelType getModelType(Model_Type type) {
	 switch (type)
	 {
	 case LOCALAI_MODEL_TEXT_GEN:
		 return BuildInModelType::LLM;
	 case LOCALAI_MODEL_EMBEDDING:
		 return BuildInModelType::Embedding;
	 case LOCALAI_MODEL_ASR:
		 return BuildInModelType::ASR;
	 case LOCALAI_MODEL_VISION:
		 return BuildInModelType::Vision;
	 default:
		 break;
	 }
 }

#define LOCALAI_CHECK_CTX() \
		do{\
			if (!g_context) {\
				return LocalAI_CreateStatus(LocalAI_ErrorCode::LOCALAI_NOT_INITIALIZED, \
				"The core context has not been initialized."); \
			}\
		}while(false)

static LocalAI_Status_t *StatusConvert(Status status) {
	if (status) {
		return nullptr;
	}
    return LocalAI_CreateStatus(status.code, status.error.c_str());
}


LOCALAI_API LocalAI_Status* LocalAI_TextResult_Free(LocalAI_TextResult* r)
{
	return nullptr;
}

LOCALAI_API LocalAI_Status* LocalAI_EmbeddingResult_Free(LocalAI_EmbeddingResult* r)
{
	if (r) {
		free(r->data);
		r->data = nullptr;
		r->length = 0;
	}

	return nullptr;
}

LOCALAI_API LocalAI_Status* LocalAI_VisionResult_Free(LocalAI_VisionResult* r)
{
	return nullptr;
}

LocalAI_Status* LocalAI_Core_Initialize(LocalAI_Config config) {

	Status status{};
	std::call_once(g_context_flag, [&]() {
		g_context = std::make_unique<LocalAI_Context_t>(config);
		if (!g_context) {
			status.code = LocalAI_ErrorCode::LOCALAI_OOM;
			status.error = "Failed to allocate memory for the context.";
		}
	});

	return StatusConvert(status);
}

LOCALAI_API LocalAI_Status* LocalAI_Core_Shutdown(void)
{
	g_context.reset();
	return nullptr;
}
//
//LOCALAI_API LocalAI_Context* LocalAI_Core_GetContext(void)
//{
//	return g_context;
//}

//LOCALAI_API LocalAI_Context* LocalAI_Core_Create(LocalAI_Context_Params params)
//{
//	auto context = std::make_unique<LocalAI_Context>(params);
//	return context.release();
//}
//
//LOCALAI_API LocalAI_Status* LocalAI_Core_Initialize(LocalAI_Context* ctx)
//{
//	LOCALAI_CHECK_CTX();
//    ctx->Initialize();
//	return nullptr;
//}
//
//LOCALAI_API LocalAI_Status* LocalAI_Core_Destroy(void)
//{
//	return nullptr;
//}

LOCALAI_API LocalAI_Status* LocalAI_Core_InitializeModel(
	Model_Type model, const char* model_path, Model_Params params)
{
	LOCALAI_CHECK_CTX();
	
	return StatusConvert(
		g_context->model_hub_->initializeModel(model, model_path, params));
}

LOCALAI_API LocalAI_Status* LocalAI_Core_Update_TextModelParams(Model_Params params)
{
	LOCALAI_CHECK_CTX();

	return StatusConvert(
		g_context->model_hub_->updateModelParams(LOCALAI_MODEL_TEXT_GEN, params));
}

LOCALAI_API LocalAI_Status* LocalAI_Core_Update_ASRModelParams(Model_Params params)
{
	LOCALAI_CHECK_CTX();

	return StatusConvert(
		g_context->model_hub_->updateModelParams(LOCALAI_MODEL_ASR, params));
}

LOCALAI_API LocalAI_Status* LocalAI_Core_Update_VisionModelParams(Model_Params params)
{
	LOCALAI_CHECK_CTX();

	return StatusConvert(
		g_context->model_hub_->updateModelParams(LOCALAI_MODEL_VISION, params));
}

LOCALAI_API LocalAI_Status* LocalAI_Core_Update_EmbeddingModelParams(Model_Params params)
{
	LOCALAI_CHECK_CTX();

	return StatusConvert(
		g_context->model_hub_->updateModelParams(LOCALAI_MODEL_EMBEDDING, params));
}

LOCALAI_API LocalAI_Status* LocalAI_GenerateAsync(const char* prompt, const char* params_json, LocalAI_TextCallback cb, void* user_data, LocalAI_Request** out_request)
{
	return nullptr;
}

LOCALAI_API LocalAI_Status* LocalAI_Embed(const char* text, LocalAI_EmbeddingResult* result)
{
	if(!text || !result)
		return LocalAI_CreateStatus(LocalAI_ErrorCode::LOCALAI_INVALID_ARG, "Invalid text input for embedding.");
	
	return StatusConvert(g_context->embeddingProvider()->embedText(text, result));
}

LOCALAI_API LocalAI_Status* LocalAI_VisionAsync(const uint8_t* image_bytes, size_t byte_length, int width, int height, int channels, const char* params_json, LocalAI_VisionCallback cb, void* user_data, LocalAI_Request** out_request)
{
	return nullptr;
}

const char* LocalAI_GetModelName(Model_Type type)
{
	if (!g_context)
		return nullptr;

	return g_context->model_hub_->getModelName(type);
}

bool LocalAI_EnumSupportedModels(LocalAI_ModelInfoCallback callback, void* user_data)
{
	if (!g_context || !callback)
		return false;

	auto models = g_context->model_hub_->supportedModels();

	for (const auto &model_id: models) {
		if (!callback(model_id, user_data))
			break;
	}

	return true;
}

LocalAI_Context_t::LocalAI_Context_t(const LocalAI_Config& config)
	:config_(config)
{
	model_hub_ = std::make_unique<AIModelHub>();
	llm_engine_ = std::make_unique<LLMEngine>(model_hub_.get());

	conv_ = std::make_unique<ConversationManager>();
	rag_ = std::make_unique<RagRetriever>(model_hub_.get());
	pm_ = std::make_unique<PluginManager>();

	orch_ = std::make_unique<Orchestrator>(conv_.get(), rag_.get(),
		llm_engine_.get(), pm_.get());

	pm_->loadPlugin(config.plugin_path);

	// Add build-in models
	model_hub_->addModel(
		Model_Type::LOCALAI_MODEL_TEXT_GEN,
		ModelProviderFactory::createBuildInModel(
			BuildInModelType::LLM));

	model_hub_->addModel(
		Model_Type::LOCALAI_MODEL_EMBEDDING,
		ModelProviderFactory::createBuildInModel(
			BuildInModelType::Embedding, config.emb_n_threads));

	model_hub_->addModel(
		Model_Type::LOCALAI_MODEL_ASR,
		ModelProviderFactory::createBuildInModel(
			BuildInModelType::ASR));

	model_hub_->addModel(
		Model_Type::LOCALAI_MODEL_VISION,
		ModelProviderFactory::createBuildInModel(
			BuildInModelType::Vision, config.vision_n_threads));
}
