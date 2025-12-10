/*
* this file implements the asynchrounous task queue for inference engine
* worker thread executes inference tasks from the queue
* To ensure thread safety, mutexes and condition variables are used
*/

#pragma once
#include <string>
#include <future>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <memory>

#include "core_common.h"
#include "core_export.h"
#include "AIModelHub.h"
#include "IModelProvider.h"
#include "ThreadPool.h"

template<class T>
struct InferenceResult{
    bool ok;
    LocalAI_ErrorCode code; 
    std::string msg;
    T payload;
};

/**
 * InferenceEngine class manages asynchronous inference tasks
 * for large language models (LLMs) and embedding models.
 */
class InferenceEngine {
public:
    InferenceEngine(
        int llm_n_threads = DEFAULT_N_LLM_THREAD,
        int emb_n_threads = DEFAULT_N_EMBEDDING_THREAD, 
        int vision_n_threads = DEFAULT_N_VISION_THREAD
    );
    ~InferenceEngine();

    std::future<InferenceResult<std::string>> submitLLM(const std::string &prompt);
    std::future<InferenceResult<std::string>> submitASR(std::span<const float> samples);
    std::future<InferenceResult<std::span<const float>>> submitEmbedding(const std::string& text);
    std::future<InferenceResult<std::string>> submitVision(const ModelProvider::ImageView &image);

    void start();
    void stop();

private:
    ThreadPool llm_pool_;
    ThreadPool emb_pool_;
    ThreadPool vision_pool_;
};