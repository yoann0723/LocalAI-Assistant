#include "core_c_api.h"
#include "AIModelHub.h"
#include "InferenceEngine.h"

 static LocalAI_Context_t* g_context = nullptr;
 static std::once_flag g_context_flag;

#define LOCALAI_CHECK_CTX() \
		do{\
			if (!g_context) {\
				return LocalAI_CreateStatus(LocalAI_ErrorCode::LOCALAI_NOT_INITIALIZED, \
				"The core context has not been initialized."); \
			}\
		}while(false)

static LocalAI_Status_t *StatusConvert(Status_Internal status) {
	if (status.code == LOCALAI_OK) {
		return nullptr;
	}
    return LocalAI_CreateStatus(status.code, status.msg.c_str());
}

struct LocalAI_Context_t {
	explicit LocalAI_Context_t(const LocalAI_Context_Params& params) :params_(params) 
	{
		/** Create model hub and inference engine */
		AIModelHub::instance();
		engine_ = std::make_unique<InferenceEngine>(
			params_.ctx_config.llm_n_threads, 
			params_.ctx_config.emb_n_threads, 
			params_.ctx_config.vision_n_threads
		);
	}

	~LocalAI_Context_t()
	{
		if (engine_) {
			engine_->stop();
		}
	}

	bool Initialize() {
		/** Load and initialize Models by given parameters */
		AIModelHub::instance().initializeModel(ID_LLM, params_.ctx_config.llm_model_path, params_.llm);
		AIModelHub::instance().initializeModel(ID_EMBEDDING_MODEL, params_.ctx_config.embedding_model_path, params_.em);
		AIModelHub::instance().initializeModel(ID_ASR_MODEL, params_.ctx_config.asr_model_path, params_.asr);
		AIModelHub::instance().initializeModel(ID_VISION_MODEL, params_.ctx_config.vision_model_path, params_.vm);
	}

	std::unique_ptr<InferenceEngine> engine_;
	LocalAI_Context_Params params_;
};


LocalAI_Status* LocalAI_Core_Initialize(LocalAI_Context_Params params) {

	Status_Internal status{};
	std::call_once(g_context_flag, [&]() {
		g_context = new (std::nothrow) LocalAI_Context_t(params);
		if (!g_context) {
			status.code = LocalAI_ErrorCode::LOCALAI_OOM;
			status.msg = "Failed to allocate memory for the context.";
		}
	});

	return StatusConvert(status);
}

LOCALAI_API LocalAI_Status* LocalAI_Core_Shutdown(void)
{
	delete g_context;
	g_context = nullptr;

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

LOCALAI_API LocalAI_Status* LocalAI_Core_Update_TextModelParams(Model_Params params)
{
	LOCALAI_CHECK_CTX();

	return StatusConvert(AIModelHub::instance().updateModelParams(ID_LLM, params));
}

LOCALAI_API LocalAI_Status* LocalAI_Core_Update_ASRModelParams(Model_Params params)
{
	LOCALAI_CHECK_CTX();

	return StatusConvert(AIModelHub::instance().updateModelParams(ID_ASR_MODEL, params));
}

LOCALAI_API LocalAI_Status* LocalAI_Core_Update_VisionModelParams(Model_Params params)
{
	LOCALAI_CHECK_CTX();

	return StatusConvert(AIModelHub::instance().updateModelParams(ID_VISION_MODEL, params));
}

LOCALAI_API LocalAI_Status* LocalAI_Core_Update_EmbeddingModelParams(Model_Params params)
{
	LOCALAI_CHECK_CTX();

	return StatusConvert(AIModelHub::instance().updateModelParams(ID_EMBEDDING_MODEL, params));
}

LOCALAI_API LocalAI_Status* LocalAI_GenerateAsync(const char* prompt, const char* params_json, LocalAI_TextCallback cb, void* user_data, LocalAI_Request** out_request)
{
	return nullptr;
}

LOCALAI_API LocalAI_Status* LocalAI_EmbedAsync(const char* text, LocalAI_EmbeddingCallback cb, void* user_data, LocalAI_Request** out_request)
{
	return nullptr;
}

LOCALAI_API LocalAI_Status* LocalAI_VisionAsync(const uint8_t* image_bytes, size_t byte_length, int width, int height, int channels, const char* params_json, LocalAI_VisionCallback cb, void* user_data, LocalAI_Request** out_request)
{
	return nullptr;
}

LocalAI_ModelInfo* LocalAI_GetModelInfo(const char * model_id)
{
	if (!g_context || !model_id)
		return nullptr;

	LocalAI_ModelInfo *info = new (std::nothrow) LocalAI_ModelInfo();
	if (!info) {
		fprintf(stderr, "Failed to allocate memory for model info");
		return nullptr;
	}

	*info = AIModelHub::instance().getModelInfo(model_id);

	return info;
}

void LocalAI_DestroyModelInfo(LocalAI_ModelInfo** info) 
{
	if (*info) {
		delete *info;
		*info = nullptr;
	}
}

bool LocalAI_EnumSupportedModels(LocalAI_ModelInfoCallback callback, void* user_data)
{
	if (!g_context || !callback)
		return false;

	auto models = AIModelHub::instance().supportedModels();
	std::for_each(models.cbegin(), models.cend(), [callback, user_data](const std::string& model_id) {
		callback(model_id.c_str(), user_data);
	});
}