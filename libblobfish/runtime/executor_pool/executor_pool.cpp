#include "executor_pool.hpp"

#include <thread>

#include "runtime/runtime.hpp"

namespace blobfish::runtime::executor_pool {

ExecutorPool::ExecutorPool(size_t thread_count):
	idle_(0)
{
	executors_.reserve(thread_count);
	for (size_t i = 0; i < thread_count; ++i) {
		executors_.emplace_back(Executor::Create(idle_, i));
	}
}

std::shared_ptr<Executor> ExecutorPool::AqcuireExecutor() {
	idle_.acquire();
	for (auto& executor: executors_) {
		if (executor->IsIdle()) {
			return executor;
		}
	}
	// not supposed to happen
	idle_.release();
	return nullptr;
}

std::shared_ptr<Executor> Executor::Create(std::counting_semaphore<1024>& idle, size_t id) {
	auto ptr = std::shared_ptr<Executor>(new Executor(idle, id));
	std::thread {
		[ptr]() {
			ptr->Run();
		}
	}.detach();
	return ptr;
}

Executor::Executor(std::counting_semaphore<1024> &idle, size_t id): idle_{idle}, id_{id} {
}

void Executor::Run() {
	while (true) {
		idle_.release();
		std::unique_lock<std::mutex> lock(m_);
		cv_.wait(lock, [this]{ return task_ != nullptr; });
		task_->Resume();
		task_ = nullptr;
	}
}
void Executor::RunTask(std::shared_ptr<task::TaskBase> task) {
	std::unique_lock<std::mutex> lock(m_);
	task_ = task;
	cv_.notify_one();
}

bool Executor::IsIdle() {
	std::unique_lock<std::mutex> lock(m_, std::defer_lock);
	if (lock.try_lock()) {
		return task_ == nullptr;
	}
	return false;
}

}