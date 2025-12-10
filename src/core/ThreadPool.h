// ThreadPool.h (improved)
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

class ThreadPool {
public:
    explicit ThreadPool(size_t threadCount = std::thread::hardware_concurrency());
    ~ThreadPool();

    void stop();

    // submit callable with args
    template<class F, class... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>
    {
        using ReturnT = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>;

        // bind function + args into a single callable returning ReturnT
        auto boundTask = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        auto taskPtr = std::make_shared<std::packaged_task<ReturnT()>>(std::move(boundTask));

        std::future<ReturnT> fut = taskPtr->get_future();

        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (stop_) throw std::runtime_error("ThreadPool is stopped");
            tasks_.emplace([taskPtr]() {
                try {
                    (*taskPtr)();
                }
                catch (...) {
                    // packaged_task will propagate exception via future
                    throw;
                }
                });
        }

        cv_.notify_one();
        return fut;
    }

private:
    void workerLoop();

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> stop_{ false };
};
