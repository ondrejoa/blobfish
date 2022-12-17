#pragma once

#include "runtime/runtime.hpp"
#include "runtime/task/spawn.hpp"

namespace blobfish::runtime::task {

template<typename T>
TaskId Spawn(Task<T>&& task) {
	auto task_ptr = std::shared_ptr<TaskBase>(new Task<T>(std::move(task)));
	auto id = blobfish::runtime::Runtime::Instance().AddTask(task_ptr);
	blobfish::runtime::Runtime::Instance().MarkAsReady(id);
	return id;
}


}