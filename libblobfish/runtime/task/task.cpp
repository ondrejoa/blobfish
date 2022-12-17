#include "task.hpp"

#include "runtime/runtime.hpp"

namespace blobfish::runtime::task::internal {

task::TaskId AddTask(std::shared_ptr<task::TaskBase> task) {
	return blobfish::runtime::Runtime::Instance().AddTask(task);
}

void MarkAsReady(task::TaskId id) {
	blobfish::runtime::Runtime::Instance().MarkAsReady(id);
}

void MarkForRemove(task::TaskId id) {
	blobfish::runtime::Runtime::Instance().MarkForRemove(id);
}


}