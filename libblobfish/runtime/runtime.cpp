#include "runtime.hpp"

#include <vector>
#include <memory>
#include <utility>
#include <stdexcept>
#include <set>
#include <iostream>
#include "runtime/task_tracker/task_tracker.hpp"
#include "runtime/executor_pool/executor_pool.hpp"

namespace blobfish::runtime {

class RuntimeImpl {
public:
	RuntimeImpl(RuntimeConfig config);

	task::TaskId AddTask(std::shared_ptr<task::TaskBase> task);
	void MarkAsReady(task::TaskId id);
	void MarkForRemove(task::TaskId id);

	[[noreturn]] int Run();
private:
	const RuntimeConfig config_;

	task_tracker::TaskTracker tracker_;
	executor_pool::ExecutorPool executor_pool_;
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

task::TaskId Runtime::AddTask(std::shared_ptr<task::TaskBase> task) {
	return pimpl_->AddTask(std::move(task));
}

void Runtime::MarkAsReady(task::TaskId id) {
	pimpl_->MarkAsReady(id);
}

void Runtime::MarkForRemove(task::TaskId id) {
	pimpl_->MarkForRemove(id);
}

// pimpl

RuntimeImpl::RuntimeImpl(blobfish::runtime::RuntimeConfig config):
	config_{config},
	tracker_(config.max_coroutines),
	executor_pool_(config.net_io_threads) // TODO: unify threads
{
}

task::TaskId RuntimeImpl::AddTask(std::shared_ptr<task::TaskBase> task) {
	return tracker_.AddTask(task);
}

void RuntimeImpl::MarkAsReady(task::TaskId id) {
	tracker_.MarkAsReady(id);
}

int RuntimeImpl::Run() {
	while (true) {
		if (auto executor = executor_pool_.AqcuireExecutor(); executor) {
			std::optional<task_tracker::TaskTracker::TaskPtr> task;
			while (!task) {
				task = tracker_.NextReady();
			}
			executor->RunTask(*task);
		}
	}
	return 0;
}


void RuntimeImpl::MarkForRemove(task::TaskId id) {
	tracker_.MarkForRemove(id);
}

}