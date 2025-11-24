#include "InferenceEngine.h"

InferenceEngine::InferenceEngine() {
    workerThread = std::thread(&InferenceEngine::workerThreadFunc, this);
}

InferenceEngine::~InferenceEngine() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stopWorker = true;
    }
    conditionVar.notify_all();
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

void InferenceEngine::submitLLM(const std::string& prompt, std::promise<std::string> resultPromise) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
		auto promise_ptr = std::make_shared<std::promise<std::string>>(std::move(resultPromise));
        taskQueue.push([prompt, promise = promise_ptr]() mutable {
            // Simulate LLM inference (replace with actual model call)
            std::string result = "LLM response for prompt: " + prompt;
            promise->set_value(result);
        });
    }
    conditionVar.notify_one();
}

void InferenceEngine::submitEmbedding(const std::string& text, std::promise<std::vector<float>> resultPromise) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
		auto promise_ptr = std::make_shared<std::promise<std::vector<float>>>(std::move(resultPromise));
        taskQueue.push([text, promise = promise_ptr]() mutable {
            // Simulate embedding generation (replace with actual model call)
            std::vector<float> embedding = {0.1f, 0.2f, 0.3f}; // Dummy embedding
            promise->set_value(embedding);
        });
    }
    conditionVar.notify_one();
}

void InferenceEngine::workerThreadFunc() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            conditionVar.wait(lock, [this]() { return !taskQueue.empty() || stopWorker; });
            if (stopWorker && taskQueue.empty()) {
                return;
            }
            task = std::move(taskQueue.front());
            taskQueue.pop();
        }
        task();
    }
}