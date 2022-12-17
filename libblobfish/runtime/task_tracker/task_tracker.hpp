#pragma once

#include <cstdlib>
#include <vector>
#include <memory>
#include <mutex>
#include <set>
#include <optional>
#include <semaphore>

#include "runtime/task/task.hpp"

namespace blobfish::runtime::task_tracker {

/**
 * Keeps track of existing tasks and their ready state
 */
class TaskTracker {
public:
	using TaskPtr = std::shared_ptr<task::TaskBase>;

	TaskTracker(size_t max_tasks);

	task::TaskId AddTask(TaskPtr task);
	void MarkAsReady(task::TaskId id);
	void MarkForRemove(task::TaskId id);

	std::optional<TaskPtr> NextReady();
private:
	const size_t max_tasks_;

	std::mutex tasks_mutex_;
	std::vector<TaskPtr> tasks_;

	std::mutex ready_mutex_;
	std::set<task::TaskId> ready_;

	std::counting_semaphore<4096> ready_count_;


	//std::mutex marked_mutex_;
	//std::set<task::TaskId> marked_;
};


}