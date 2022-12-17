#include "runtime.hpp"

#include <vector>
#include <memory>
#include <utility>
#include <stdexcept>
#include <set>
#include <iostream>

namespace blobfish::runtime {

class RuntimeImpl {
public:
	RuntimeImpl(RuntimeConfig config);

	task::TaskId AddTask(std::shared_ptr<task::TaskBase> task, Executor executor);
	void MarkAsready(task::TaskId id);

	[[noreturn]] int Run();
private:
	const RuntimeConfig config_;

	// TODO: make it maintainable
	// index corresponds to TaskId
	std::vector<std::shared_ptr<task::TaskBase>> tasks_;

	// TODO: make it more effective/easier to schedule
	std::set<task::TaskId> tasks_ready_;

	void ResumeTask(task::TaskId id);
	void ExecuteReady();
};

Runtime::Runtime() {}

Runtime::~Runtime() {}

Runtime &Runtime::Instance() {
	static Runtime runtime;
	return runtime;
}

void Runtime::InitRuntime(RuntimeConfig config) {
	pimpl_ = std::make_unique<RuntimeImpl>(config);
}

int Runtime::Run() {
	return pimpl_->Run();
}

task::TaskId Runtime::AddTask(std::shared_ptr<task::TaskBase> task, Executor executor) {
	return pimpl_->AddTask(std::move(task), executor);
}

void Runtime::MarkAsReady(task::TaskId id) {
	pimpl_->MarkAsready(id);
}

// pimpl

RuntimeImpl::RuntimeImpl(blobfish::runtime::RuntimeConfig config): config_{config} {
	tasks_.reserve(config_.max_coroutines);
	for (size_t i = 0; i < config_.max_coroutines; ++i) {
		tasks_.emplace_back(nullptr);
	}
}

task::TaskId RuntimeImpl::AddTask(std::shared_ptr<task::TaskBase> task, blobfish::runtime::Executor executor) {
	for (size_t i = 0; i < config_.max_coroutines; ++i) {
		if (tasks_[i] == nullptr) {
			// TODO set fields
			task->SetId(task::TaskId{i});
			tasks_[i] = task;
			return task::TaskId {i};
		}
	}
	throw std::runtime_error("task list full");
}

void RuntimeImpl::MarkAsready(task::TaskId id) {
	// TODO protect with lock
	tasks_ready_.insert(id);
}

void RuntimeImpl::ResumeTask(task::TaskId id) {
	// TODO schedule
	tasks_[size_t(id)]->Resume();
}

int RuntimeImpl::Run() {
	while (true) {
		ExecuteReady();
	}
	return 0;
}

void RuntimeImpl::ExecuteReady() {
	std::set<task::TaskId> tasks_ready = std::move(tasks_ready_);
	for (auto id: tasks_ready) {
		ResumeTask(id);
	}
}

}