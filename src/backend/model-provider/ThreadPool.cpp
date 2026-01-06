#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t threadCount, const std::string& name)
	:thread_name_(name)
{
	if (threadCount == 0) threadCount = 1;
	for (size_t i = 0; i < threadCount; ++i) {
		workers_.emplace_back(&ThreadPool::workerLoop, this);
	}
}

ThreadPool::~ThreadPool() {
	stop();
}

void ThreadPool::stop() 
{
	if (stop_)
		return;

	stop_ = true;
	cv_.notify_all();
	for (auto& t : workers_) {
		if (t.joinable()) t.join();
	}
}

void ThreadPool::run(Task task)
{
	{
		std::lock_guard<std::mutex> lock(mutex_);
		tasks_.push(std::move(task));
	}
	cv_.notify_one();
}

thread_local int id;
void ThreadPool::workerLoop() 
{
	id = id_++;
	fprintf(stderr, "thread: `%s-%d` for `%s` started.", __FUNCTION__, id, thread_name_.c_str());
	while (!stop_) {
		Task task;
		{
			std::unique_lock<std::mutex> lock(mutex_);
			cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
			if (stop_ && tasks_.empty()) return;

			task = std::move(tasks_.front());
			tasks_.pop();
		}
		task();
	}
	fprintf(stderr, "thread: `%s-%d` for `%s` ended.", __FUNCTION__, id, thread_name_.c_str());
}
