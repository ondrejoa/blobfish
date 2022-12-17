#include "task_tracker.hpp"

#include <iostream>

namespace blobfish::runtime::task_tracker {

TaskTracker::TaskTracker(size_t max_tasks): max_tasks_{max_tasks}, ready_count_{0} {
	tasks_.resize(max_tasks_, nullptr);
}

task::TaskId TaskTracker::AddTask(TaskTracker::TaskPtr task) {
	std::scoped_lock lock(tasks_mutex_);
	for (size_t i = 0; i < max_tasks_; ++i) {
		if (tasks_[i] == nullptr) {
			task->SetId(task::TaskId{i});
			tasks_[i] = task;
			return task::TaskId {i};
		}
	}
	// TODO: terminate
}

void TaskTracker::MarkAsReady(task::TaskId id) {
	ready_count_.release();
	std::scoped_lock lock(ready_mutex_);
	ready_.insert(id);
}

void TaskTracker::MarkForRemove(task::TaskId id) {
	std::scoped_lock lock(tasks_mutex_);
	tasks_[size_t(id)] = nullptr;
}

std::optional<TaskTracker::TaskPtr> TaskTracker::NextReady() {
	ready_count_.acquire();
	std::scoped_lock lock(tasks_mutex_, ready_mutex_); // Az elso lock nem kotelezo
	for (auto id: ready_) {
		ready_.erase(id); // ehh
		return tasks_[size_t(id)];
	}
	return std::nullopt;
}

}