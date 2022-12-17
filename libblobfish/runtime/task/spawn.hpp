#pragma once

#include "runtime/runtime.hpp"
#include "runtime/task/spawn.hpp"

namespace blobfish::runtime::task {

template<typename T>
TaskId Spawn(Task<T>&& task, blobfish::runtime::Executor executor) {
	auto task_ptr = std::shared_ptr<TaskBase>(new Task<T>(std::move(task)));
	auto id = blobfish::runtime::Runtime::Instance().AddTask(task_ptr, executor);
	blobfish::runtime::Runtime::Instance().MarkAsReady(id);
	return id;
}

template<typename T>
TaskId SpawnIO(Task<T>&& task) {
	return Spawn(std::move(task), blobfish::runtime::Executor::IO);
}

template<typename T>
TaskId SpawnCompute(Task<T>&& task) {
	return Spawn(std::move(task), blobfish::runtime::Executor::Compute);
}

}