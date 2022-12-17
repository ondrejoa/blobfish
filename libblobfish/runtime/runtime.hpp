#pragma once

#include <memory>

#include "runtime_config.hpp"

#include "task/task_defs.hpp"
#include "task/task.hpp"

namespace blobfish::runtime {

class RuntimeImpl;


class Runtime final {
	friend class RuntimeBuilder;
public:
	~Runtime();

	int Run();

	static Runtime& Instance();

	task::TaskId AddTask(std::shared_ptr<task::TaskBase> task);
	void MarkAsReady(task::TaskId id);
	void MarkForRemove(task::TaskId id);

private:
	Runtime();
	void InitRuntime([[maybe_unused]] RuntimeConfig config);

	std::unique_ptr<RuntimeImpl> pimpl_;
};

}