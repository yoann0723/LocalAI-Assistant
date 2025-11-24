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

/**
 * InferenceEngine class manages asynchronous inference tasks
 * for large language models (LLMs) and embedding models.
 */
class InferenceEngine {
public:
    InferenceEngine();
    ~InferenceEngine();

    void submitLLM(const std::string& prompt, std::promise<std::string> resultPromise);
    void submitEmbedding(const std::string& text, std::promise<std::vector<float>> resultPromise);

private:
    void workerThreadFunc();

    std::thread workerThread;
    std::queue<std::function<void()>> taskQueue;
    std::mutex queueMutex;
    std::condition_variable conditionVar;
    bool stopWorker = false;
};