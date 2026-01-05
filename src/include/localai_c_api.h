/*****************************************************************//**
 * \file   core_c_api.h
 * \brief  core C API header file
 * 
 * \author yoann
 * \date   December 2025
 *********************************************************************/
#ifndef LOCALAI_C_API_H
#define LOCALAI_C_API_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>
#include <stdint.h>

    /* ---------- Export macro ---------- */
#if defined(_WIN32)
#if defined(CORE_BUILD)
#define LOCALAI_API __declspec(dllexport)
#else
#define LOCALAI_API __declspec(dllimport)
#endif
#else
#define LOCALAI_API
#endif

/* ---------- Version ---------- */
/* Returns a NUL-terminated static string owned by the library. */
LOCALAI_API const char* Core_GetVersion(void);

/* ---------- Opaque handles ---------- */
/* Core instance: manages workers, models, queues */
typedef struct LocalAI_Context_t LocalAI_Context;

/* Request handle: returned by async submit calls */
typedef struct LocalAI_Request_t LocalAI_Request;

/* Satus handle: contains error code and error message */
typedef struct LocalAI_Status_t LocalAI_Status;

typedef struct LocalAI_ChatSession_t LocalAI_ChatSession;

typedef struct LocalAI_Text_Model_t LocalAI_Text_Model;

typedef struct LocalAI_ASR_Model_t LocalAI_ASR_Model;

typedef struct LocalAI_Embedding_Model_t LocalAI_Embedding_Model;

typedef struct LocalAI_Vision_Model_t LocalAI_Vision_Model;

typedef enum {
    LOCALAI_OK = 0,
    LOCALAI_UNKNOWN,
    LOCALAI_NOT_INITIALIZED,
    LOCALAI_INVALID_ARG,
    LOCALAI_NO_MODEL,
    LOCALAI_MODEL_LOAD_FAILED,
    LOCALAI_BUSY,
    LOCALAI_OOM,
    LOCALAI_CANCELLED,
    LOCALAI_TIMEOUT,
    LOCALAI_RUNTIME_ERROR,
    LOCALAI_MODEL_TOKENIZE_ERROR,
    LOCALAI_MODEL_DECODE_ERROR,
    LOCALAI_MODEL_GENERATE_ERROR,
    LOCALAI_INTERNAL = 100
} LocalAI_ErrorCode;

typedef enum {
    LOCALAI_LOG_LEVEL_ERROR = 0,
    LOCALAI_LOG_LEVEL_WARN = 1,
    LOCALAI_LOG_LEVEL_INFO = 2,
	LOCALAI_LOG_LEVEL_DEBUG = 3
}LogLevel;

typedef enum {
    LOCALAI_REQUEST_PENDING,
    LOCALAI_REQUEST_RUNNING,
    LOCALAI_REQUEST_CANCELLED,
    LOCALAI_REQUEST_COMPLETED,
    LOCALAI_REQUEST_FAILED
}LocalAI_RequestStatus;

/**
 * Construct a pointer to LocalAI_Status with error code and msg.
 * 
 * LocalAI_StatusDestroy should be called to release the momory properly.  
 * 
 * \return Returns null if OOM otherwise allocate momery and copy the code and msg to it. 
 */
LOCALAI_API LocalAI_Status* LocalAI_CreateStatus(LocalAI_ErrorCode code, const char* msg);
LOCALAI_API LocalAI_ErrorCode LocalAI_StatusGetCode(const LocalAI_Status* st);
LOCALAI_API const char* LocalAI_StatusGetMessage(const LocalAI_Status* st);
LOCALAI_API void LocalAI_StatusDestroy(LocalAI_Status* st);


typedef enum {
    LOCALAI_MODEL_TEXT_GEN = 1,// Text-generation model
    LOCALAI_MODEL_EMBEDDING = 2,
    LOCALAI_MODEL_ASR = 3, // Automantic Speech Recognization 
	LOCALAI_MODEL_VISION = 4, // vision model
    LOCALAI_MODEL_TTS = 5, // text-to-speech
    LOCALAI_MODEL_TTI = 6 // text-to-image
}Model_Type;

typedef struct {
    int top_k;
    int temp;
    int max_new_tokens;
    int frequency_penalty;
    int presence_penalty;
    int repeat_penalty;

    int n_threads;
    int n_gpu_layers;
    int quantize;
    int n_layer;
    int n_embd;
}Model_Params;

typedef bool (*LocalAI_ModelInfoCallback)(Model_Type type, void* user_data);
LOCALAI_API bool LocalAI_EnumSupportedModels(LocalAI_ModelInfoCallback callback, void* user_data);
LOCALAI_API const char* LocalAI_GetModelName(Model_Type type);
// LOCALAI_API void LocalAI_DestroyModelInfo(LocalAI_ModelInfo **info);
// LOCALAI_API const char * LocalAI_GetModelId(LocalAI_ModelInfo *info);
// LOCALAI_API const char* LocalAI_GetModelPath(LocalAI_ModelInfo *info);
// LOCALAI_API Model_Params* LocalAI_GetModelParams(LocalAI_ModelInfo *info);

typedef struct {
    const char* llm_model_path;
    const char* embedding_model_path;
    const char* vision_model_path;
    const char* asr_model_path;
    const char* plugin_path;

    bool enable_asr;
    bool enable_vision;
    int llm_n_threads;
    int emb_n_threads;
    int vision_n_threads;
}LocalAI_Config;

typedef struct{
    Model_Params llm;
    Model_Params em;
    Model_Params vm;
    Model_Params asr;

    LocalAI_Config ctx_config;
}LocalAI_Context_Params;

/* Text (LLM) result */
typedef struct {
    char* text; /* NUL-terminated, heap allocated by library (malloc or compatible) */
} LocalAI_TextResult;
LOCALAI_API LocalAI_Status* LocalAI_TextResult_Free(const LocalAI_TextResult* r);

/* Embedding result */
typedef struct {
    float* data;    /* heap-allocated float array */
    size_t length;  /* number of floats */
} LocalAI_EmbeddingResult;
LOCALAI_API LocalAI_Status* LocalAI_EmbeddingResult_Free(LocalAI_EmbeddingResult* r);

/* Vision result (caption / OCR text) */
typedef struct {
    char* text; /* NUL-terminated */
} LocalAI_VisionResult;
LOCALAI_API LocalAI_Status* LocalAI_VisionResult_Free(LocalAI_VisionResult* r);

/* ---------- Callback types for async APIs ----------
   - Callbacks may be invoked on internal worker threads. The callback implementer must be thread-safe.
   - user_data is forwarded from submit call.
*/
typedef void (*LocalAI_TextCallback)(LocalAI_Request* request, const LocalAI_TextResult* result, LocalAI_Status* status, void* user_data);
typedef void (*LocalAI_EmbeddingCallback)(LocalAI_Request* request, const LocalAI_EmbeddingResult* result, LocalAI_Status* status, void* user_data);
typedef void (*LocalAI_VisionCallback)(LocalAI_Request* request, const LocalAI_VisionResult* result, LocalAI_Status* status, void* user_data);

/**
 * Create and initialize core context. Allocate all the momery needed for the models
 * \param [in] context parameters and models' parameters
 */
LOCALAI_API LocalAI_Status* LocalAI_Core_Initialize(LocalAI_Config config);
LOCALAI_API LocalAI_Status* LocalAI_Core_Shutdown(void);

LOCALAI_API LocalAI_Status* LocalAI_Core_InitializeModel(Model_Type model, const char *model_path, 
                                                         Model_Params params);

LOCALAI_API LocalAI_Status* LocalAI_Core_CreateChatSession(LocalAI_ChatSession** out_session);

LOCALAI_API LocalAI_Status* LocalAI_ASR_Model_Create(const char* model_path,
    Model_Params params, LocalAI_ASR_Model** out_model);

LOCALAI_API void LocalAI_ASR_Model_Release(LocalAI_ASR_Model* model);

LOCALAI_API LocalAI_Status* LocalAI_Text_Model_Create(const char* model_path,
    Model_Params params, LocalAI_Text_Model** out_model);

LOCALAI_API void LocalAI_Text_Model_Release(LocalAI_Text_Model *model);

LOCALAI_API LocalAI_Status* LocalAI_Embedding_Model_Create(const char* model_path,
    Model_Params params, LocalAI_Embedding_Model** out_model);

LOCALAI_API void LocalAI_Embedding_Model_Release(LocalAI_Embedding_Model *model);

LOCALAI_API LocalAI_Status* LocalAI_Vision_Model_Create(const char* model_path,
    Model_Params params, LocalAI_Vision_Model** out_model);

LOCALAI_API void LocalAI_Vision_Model_Release(LocalAI_Vision_Model* model);

typedef enum {
    ASR_STOPPED,
    ASR_RUNNING,
    ASR_PAUSED
}ASRStatus;

typedef struct {
    void* user_data;
    int sample_rate;
    bool (*fill_buffer)(float* buffer, int buffer_size, int ms, void* user_data);
    void (*on_heard)(const char* text, int len, void* user_data);
    void (*on_status_changed)(ASRStatus status, void* user_data);
    void (*on_error)(LocalAI_ErrorCode code, const char *error, void* user_data);
}LocalAI_AudioProviderInfo;

LOCALAI_API LocalAI_Status* LocalAI_ChatEnableASR(LocalAI_ChatSession* session,
    LocalAI_AudioProviderInfo* audio_provider, LocalAI_ASR_Model *asr_model);

LOCALAI_API void LocalAI_ChatDisableASR(LocalAI_ChatSession* session);
LOCALAI_API void LocalAI_ChatResumeASR(LocalAI_ChatSession* session);
LOCALAI_API void LocalAI_ChatPauseASR(LocalAI_ChatSession* session);
LOCALAI_API void LocalAI_ChatStopASR(LocalAI_ChatSession* session);

LOCALAI_API void LocalAI_Core_ReleaseChatSession(LocalAI_ChatSession* session);

LOCALAI_API LocalAI_Status* LocalAI_Core_Update_TextModelParams(Model_Params params);
LOCALAI_API LocalAI_Status* LocalAI_Core_Update_ASRModelParams(Model_Params params);
LOCALAI_API LocalAI_Status* LocalAI_Core_Update_VisionModelParams(Model_Params params);
LOCALAI_API LocalAI_Status* LocalAI_Core_Update_EmbeddingModelParams(Model_Params params);

/* Logging: supply a callback to receive library logs (thread-safe).
 * level: 0=error,1=warn,2=info,3=debug
 */
typedef void (*LocalAI_LogCallback)(LogLevel level, const char* message, void* user_data);
LOCALAI_API void LocalAI_Core_SetLogCallback(LocalAI_LogCallback cb, void* user_data);

/* ---------- Asynchronous (non-blocking) APIs ----------
   Submits a request and returns immediately. out_request can be NULL if caller doesn't wish to control it.
   Callback will be invoked once the result is ready (or an error occurs).
   - The library guarantees the callback will be invoked exactly once for a submitted request unless the request is cancelled and cancelled callbacks are disabled.
   - The callback receives a result pointer which the callback MUST NOT free; the callback should copy data if needed; after callback returns, user should call LocalAI_RequestRelease to free request resources.
*/
LOCALAI_API LocalAI_Status* LocalAI_GenerateAsync(LocalAI_ChatSession *session, const char* prompt,
                                                  const char* params_json, LocalAI_TextCallback cb,
                                                  void* user_data, LocalAI_Request** out_request);

LOCALAI_API LocalAI_Status* LocalAI_Embed(const char* text, LocalAI_EmbeddingResult *result);

LOCALAI_API LocalAI_Status* LocalAI_VisionAsync(const uint8_t* image_bytes, 
                                                size_t byte_length,
                                                int width, int height, int channels,
                                                const char* params_json,
                                                LocalAI_VisionCallback cb, void* user_data,
                                                LocalAI_Request** out_request);

/* ---------- Request control ----------
   - Cancel: best-effort; if request is already running, cancellation attempts to stop work but may fail.
   - Wait: blocks up to timeout_ms (-1 == wait forever).
   - Status: returns 0=Pending,1=Running,2=Completed,3=Cancelled,4=Failed
   - Release: free request handle (must be called after callback notified or wait returned).
*/
LOCALAI_API LocalAI_Status* LocalAI_Request_Cancel(LocalAI_Request* request);
LOCALAI_API LocalAI_Status* LocalAI_Request_Wait(LocalAI_Request* request, int64_t timeout_ms);
LOCALAI_API LocalAI_Status* LocalAI_Request_Status(LocalAI_Request* request, int* out_status);
LOCALAI_API void LocalAI_Request_Release(LocalAI_Request* request);

#ifdef __cplusplus
}
#endif

#endif LOCALAI_C_API_H