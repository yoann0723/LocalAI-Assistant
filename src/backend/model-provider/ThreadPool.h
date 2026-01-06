#pragma once
#include <vector>
#include <thread>
#include <future>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <type_traits>
#include <utility>
#include <string_view>
#include "../common/common.hpp"

class ThreadPool {
    using Task = std::function<void()>;
public:
    explicit ThreadPool(size_t threadCount = std::thread::hardware_concurrency(),
        const std::string &name = std::string());
    ~ThreadPool();

    void stop();

    void run(Task task);

private:
    void workerLoop();

    std::vector<std::thread> workers_;
    std::queue<Task> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> stop_{ false };
    std::string thread_name_;
    std::atomic<int> id_ = 0;
};
