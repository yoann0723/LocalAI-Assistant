#include "LocalAIContext.h"
#include "../common/common.hpp"

using namespace LocalAI;

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


LOCALAI_API LocalAI_Status* LocalAI_TextResult_Free(const LocalAI_TextResult* r)
{
	if(r->text)
		free(r->text);

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

struct LocalAI_Request_t::Impl {
	std::shared_ptr<AsyncPipeline> pipeline;
};

LocalAI_Request_t::LocalAI_Request_t()
	:impl_(std::make_unique<LocalAI_Request_t::Impl>())
{}

LocalAI_Request_t::~LocalAI_Request_t()
{}

LocalAI_RequestStatus LocalAI_Request_t::status() 
{
	return impl_->pipeline->status();
}

void LocalAI_Request_t::cancel() 
{
	impl_->pipeline->cancel();
}

LOCALAI_API LocalAI_Status* LocalAI_GenerateAsync(
	LocalAI_ChatSession_t* session, const char* prompt, const char* params_json,
	LocalAI_TextCallback cb, void* user_data, 
	LocalAI_Request** out_request)
{
	if(!session || !g_context)
		return LocalAI_CreateStatus(LocalAI_ErrorCode::LOCALAI_NOT_INITIALIZED, 
			"The core context has not been initialized.");

	//session->orch_->handleUserInput(prompt, );
	if (session) {
		session->chat(prompt ? prompt : "", params_json ? params_json : "", out_request, cb, user_data);
	}
	
	return nullptr;
}

LOCALAI_API LocalAI_Status* LocalAI_Embed(
	const char* text, 
	LocalAI_EmbeddingResult* result)
{
	if(!text || !result)
		return LocalAI_CreateStatus(LocalAI_ErrorCode::LOCALAI_INVALID_ARG, 
			"Invalid text input for embedding.");
	
	return StatusConvert(g_context->embeddingProvider()->embedText(text, result));
}

LOCALAI_API LocalAI_Status* LocalAI_VisionAsync(
	const uint8_t* image_bytes, size_t byte_length, 
	int width, int height, 
	int channels, const char* params_json, 
	LocalAI_VisionCallback cb, void* user_data, 
	LocalAI_Request** out_request)
{
	return nullptr;
}

LOCALAI_API LocalAI_Status* LocalAI_Core_CreateSession(LocalAI_ChatSession_t** out_session)
{
	LOCALAI_CHECK_CTX();

	auto session = new LocalAI_ChatSession_t();
	session->impl_ = g_context->createChatSession();
	*out_session = session;

	return nullptr;
}

LOCALAI_API void LocalAI_Core_ReleaseSession(LocalAI_ChatSession* session)
{
	if (!g_context || !session)
		return;

	g_context->removeChatSession(session->impl_);

	delete session;
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

LocalAI_Status* LocalAI_Request_Cancel(LocalAI_Request* request)
{
	if (request) {
		request->cancel();
	}

	return nullptr;
}

LocalAI_Status* LocalAI_Request_Status(LocalAI_Request* request, int* out_status)
{
	if (request) {
		*out_status = request->status();
	}

	return nullptr;
}

void LocalAI_Request_Release(LocalAI_Request* request) 
{
	if (request)
		delete request;
}

LocalAI_Context_t::LocalAI_Context_t(const LocalAI_Config& config)
	:config_(config)
{
	model_hub_ = std::make_unique<AIModelHub>();
	pm_ = std::make_unique<PluginManager>();
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

LocalAI_Context_t::~LocalAI_Context_t() 
{
	std::lock_guard locker(session_mtx_);
	if (chat_sessions_.size() > 0) {
		assert(false && "Exit while chat session still alive.");
		fprintf(stderr, "Exit while chat session still alive.\n");
	}
}

ChatSessionSharedPtr LocalAI_Context_t::createChatSession()
{
	std::lock_guard locker(session_mtx_);
	auto session = std::make_shared<ChatSessionImpl>(model_hub_.get(), pm_.get());
	chat_sessions_.emplace_back(session);
	return session;
}

void LocalAI_Context_t::removeChatSession(const ChatSessionSharedPtr& session)
{
	std::lock_guard locker(session_mtx_);
	auto f = [session](const ChatSessionWeakPtr& weak_ptr) {
		auto s = weak_ptr.lock();
		if (s && s == session) {
			return true;
		}

		return false;
	};
	auto iter = std::find_if(chat_sessions_.cbegin(), chat_sessions_.cend(), f);
	if (iter != chat_sessions_.end()) {
		chat_sessions_.erase(iter);
	}
}

struct ChatSessionImpl {
	ChatSessionImpl(AIModelHub* model_hub,
		PluginManager* pm)
		:model_hub_(model_hub), pm_(pm) {

		rag_ = std::make_unique<RagRetriever>(model_hub_->embedding());
		//TODO: load capabilities from plugin manager.
		rag_->loadCapabilities({});
		fprintf(stderr, "ChatSessionImpl: `%p` constructed.\n", (void*)this);
	}

	~ChatSessionImpl() {
		fprintf(stderr, "ChatSessionImpl: `%p` destroyed.\n", (void*)this);
	}

	std::shared_ptr<AsyncPipeline> createChatPipeline() {
		auto pipeline = std::make_shared<AsyncPipeline>("Chat pipeline");
		// Step01: embedding input text and retreive top-k from all plugin's capabilities.   
		pipeline->addStep(std::make_unique<RagStep>(rag_.get()));
		// Step02: create a prompt with retreived capabilities feed it into llm.
		pipeline->addStep(std::make_unique<LLMStep>(model_hub_->llm()));
		// Step03: Call the plugins that the llm returns.
		pipeline->addStep(std::make_unique<ChatResponseStep>());
		// Last step: generate response text.
		pipeline->setFinalizer(std::make_unique<ChatFinalizer>());
		return pipeline;
	}

	std::shared_ptr<AsyncPipeline> chat(std::string_view message, std::string_view params,
		LocalAI_TextCallback callback, void* user_data) {

		auto pipeline = createChatPipeline();
		auto pipelineCtx = std::make_shared<PipelineContext>();
		pipelineCtx->prompt = message;
		pipeline->start(pipelineCtx, [callback, user_data](const Status& status, void* result) {
			if (!status) {
				fprintf(stderr, "Failed to execute the pipeline.error: %s\n", status.error.c_str());
				callback(nullptr, nullptr, StatusConvert(status), user_data);
			}
			else {
				auto r = static_cast<char*>(result);
				LocalAI_TextResult response;
				response.text = r;
				callback(nullptr, &response, StatusConvert(status), user_data);
			}
			});

		return pipeline;
	}

private:
	//std::shared_ptr<AsyncPipeline> chat_pipeline_;

	std::unique_ptr<ConversationManager> conv_;
	std::unique_ptr<LLMEngine> llm_engine_;
	std::unique_ptr<Orchestrator> orch_;
	std::unique_ptr<RagRetriever> rag_;

	AIModelHub* model_hub_;
	PluginManager* pm_;
};

//LocalAI_ChatSession_t::LocalAI_ChatSession_t(
//	std::weak_ptr<AIModelHub> model_hub,
//	std::weak_ptr<PluginManager> pm)
//{
//	llm_engine_ = std::make_unique<LLMEngine>(model_hub);
//
//	conv_ = std::make_unique<ConversationManager>();
//
//	orch_ = std::make_unique<Orchestrator>(conv_.get(), rag_.get(),
//		llm_engine_.get(), pm);
//}

void LocalAI_ChatSession_t::chat(
	std::string_view message, std::string_view params, LocalAI_Request** out_request,
	LocalAI_TextCallback callback, void* user_data)
{
	if (impl_) {
		*out_request = new LocalAI_Request_t();
		auto pipeline = impl_->chat(message, params, callback, user_data);
		(*out_request)->impl_->pipeline = pipeline;
	}
}
