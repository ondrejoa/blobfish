#include "task.hpp"

#include "runtime/runtime.hpp"

namespace blobfish::runtime::task::internal {

task::TaskId AddTask(std::shared_ptr<task::TaskBase> task, Executor executor) {
	return blobfish::runtime::Runtime::Instance().AddTask(task, executor);
}

void MarkAsReady(task::TaskId id) {
	blobfish::runtime::Runtime::Instance().MarkAsReady(id);
}


}