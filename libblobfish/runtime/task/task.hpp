#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"
#pragma once

#include <coroutine>
#include <optional>
#include <memory>

#include "task_defs.hpp"

namespace blobfish::runtime {
	class RuntimeImpl;
}

namespace blobfish::runtime::task {

// base
class TaskBase {
	friend class blobfish::runtime::RuntimeImpl;
public:
	virtual ~TaskBase() = default;

	virtual TaskId GetId() const = 0;
	virtual void SetId(TaskId id) = 0;

	virtual void Resume() = 0;
};

namespace internal {
	task::TaskId AddTask(std::shared_ptr<task::TaskBase> task);
	void MarkAsReady(task::TaskId id);
	void MarkForRemove(task::TaskId id);
}

// void return type
struct Void {};


// task
template<typename T = Void>
class Task final: public TaskBase {
public:
	template<typename U> friend class Task;

	// forward declaration
	template<typename U>
	struct SubTask;

	struct promise_type {
		using coro_handle = std::coroutine_handle<promise_type>;

		auto get_return_object() { return coro_handle::from_promise(*this); }
		auto initial_suspend() { return std::suspend_always{}; }
		auto final_suspend() noexcept {
			if (dependent) {
				internal::MarkAsReady(*dependent);
			}
			internal::MarkForRemove(task_id);
			return std::suspend_always{};
		}
		void unhandled_exception() {}

		TaskId task_id = TaskId {0};

		std::shared_ptr<std::optional<T>> value = std::shared_ptr<std::optional<T>>(new std::optional<T>(std::nullopt));

		void return_value(T t) {
			*value = std::move(t);
		}

		std::optional<TaskId> dependent = std::nullopt;

		// forward declaration
		template<typename U>
		SubTask<U> await_transform(Task<U>&&);

		// TODO remove
		std::suspend_always await_transform(std::suspend_always) {
			return std::suspend_always{};
		}
	};
	using coro_handle = typename promise_type::coro_handle;

	template<typename U>
	struct SubTask {
		task::TaskId id;
		std::shared_ptr<std::optional<U>> value;

		bool await_ready() const noexcept { return false; }
		void await_suspend(coro_handle handle) noexcept {
			internal::MarkAsReady(id);
		}

		U await_resume() const noexcept {
			return **value;
		}

	};


	Task() = default;
	Task(const Task&) = delete;
	Task(Task&& other) noexcept {
		handle_ = other.handle_;
		other.handle_ = coro_handle{};
	}
	Task(coro_handle handle): handle_{handle} {}
	Task& operator=(Task&& other) noexcept {
		if (this == &other) {
			return *this;
		}
		if (handle_) {
			handle_.destroy();
		}
		handle_ = other.handle_;
		other.handle_ = coro_handle{};
		return *this;
	}

	~Task() override {
		if (handle_) {
			handle_.destroy();
		}
	};

	TaskId GetId() const override {
		return handle_.promise().task_id;
	}

	void SetId(TaskId id) override {
		handle_.promise().task_id = id;
	}


	void Resume() override {
		handle_.resume();
	}

private:
	coro_handle handle_;
};

template<typename T>
template<typename U>
typename Task<T>::template SubTask<U> Task<T>::promise_type::await_transform(Task<U>&& task) {
	auto sub_value = task.handle_.promise().value; // Reference to return value
	task.handle_.promise().dependent = task_id; // Make this task depend on subtask
	auto task_ptr = std::shared_ptr<TaskBase>(new Task<U>(std::move(task)));
	auto id = internal::AddTask(task_ptr);
	auto subtask = SubTask<U> {
		.id = id,
		.value = sub_value,
	};
	return subtask;
}


}
#pragma clang diagnostic pop