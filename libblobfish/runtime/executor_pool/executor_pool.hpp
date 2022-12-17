#pragma once

#include <cstdlib>
#include <vector>
#include <atomic>
#include <semaphore>
#include <condition_variable>
#include <mutex>

#include "runtime/task/task.hpp"

namespace blobfish::runtime::executor_pool {

class Executor: public std::enable_shared_from_this<Executor> {
public:
	[[nodiscard]] static std::shared_ptr<Executor> Create(std::counting_semaphore<1024>& idle, size_t id);

	void RunTask(std::shared_ptr<task::TaskBase> task);
	bool IsIdle();
private:
	Executor(std::counting_semaphore<1024>& idle, size_t id);

	std::mutex m_;
	std::condition_variable cv_;
	std::shared_ptr<task::TaskBase> task_;

	std::counting_semaphore<1024>& idle_;

	const size_t id_;

	[[noreturn]] void Run();
};

class ExecutorPool final {
public:
	ExecutorPool(size_t thread_count);

	std::shared_ptr<Executor> AqcuireExecutor();
private:
	std::vector<std::shared_ptr<Executor>> executors_;
	std::counting_semaphore<1024> idle_; // looks bad
};

}