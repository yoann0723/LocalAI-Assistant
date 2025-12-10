#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t threadCount) {
	if (threadCount == 0) threadCount = 1;
	for (size_t i = 0; i < threadCount; ++i) {
		workers_.emplace_back(&ThreadPool::workerLoop, this);
	}
}

ThreadPool::~ThreadPool() {
	stop();
}

void ThreadPool::stop() {
	if (stop_)
		return;

	stop_ = true;
	cv_.notify_all();
	for (auto& t : workers_) {
		if (t.joinable()) t.join();
	}
}

void ThreadPool::workerLoop() {
	while (!stop_) {
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> lock(mutex_);
			cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
			if (stop_ && tasks_.empty()) return;

			task = std::move(tasks_.front());
			tasks_.pop();
		}
		task();
	}
}
