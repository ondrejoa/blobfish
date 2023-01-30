#pragma once

#include <coroutine>
#include <optional>
#include <memory>
#include <functional>
#include <type_traits>
#include <concepts>

#include "task_defs.hpp"
#include "awaitable.hpp"

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
class Task final : public TaskBase {
public:
	template<typename U> friend
	class Task;

	template<Awaitable A>
	struct AwaitablePromise;

	template<AwaitableValue A>
	struct AwaitableValuePromise;

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

		TaskId task_id = TaskId{0};

		std::shared_ptr<std::optional<T>> value = std::shared_ptr<std::optional<T>>(new std::optional<T>(std::nullopt));

		void return_value(T t) {
			*value = std::move(t);
		}

		std::optional<TaskId> dependent = std::nullopt;

		template<Awaitable A>
		AwaitablePromise<A> await_transform(A &&);

		template<AwaitableValue A>
		AwaitableValuePromise<A> await_transform(A &&);

		template<typename U>
		AwaitableValuePromise<typename Task<U>::AwaitableTaskValue> await_transform(Task<U>&&);
	};
	using coro_handle = typename promise_type::coro_handle;

	template<Awaitable A>
	struct AwaitablePromise {
		A awaitable;

		bool await_ready() const noexcept {
			return awaitable.Ready();
		}

		void await_suspend(coro_handle handle) noexcept {
			awaitable.Suspend(handle.promise().task_id);
		}

		void await_resume() const noexcept {}
	};

	template<AwaitableValue A>
	struct AwaitableValuePromise {
		A awaitable;

		bool await_ready() const noexcept {
			return awaitable.Ready();
		}

		void await_suspend(coro_handle handle) noexcept {
			awaitable.Suspend(handle.promise().task_id);
		}

		typename A::ValueType await_resume() const noexcept {
			return awaitable.Value();
		}
	};

	Task() = default;
	Task(const Task &) = delete;
	Task(Task &&other) noexcept {
		handle_ = other.handle_;
		other.handle_ = coro_handle{};
	}
	Task(coro_handle handle) : handle_{handle} {}
	Task &operator=(Task &&other) noexcept {
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

	struct AwaitableTaskValue {
		coro_handle handle;
		std::shared_ptr<std::optional<T>> value;

		using ValueType = T;

		bool Ready() const noexcept {
			return false;
		}

		void Suspend(TaskId dependent) noexcept {
			handle.promise().dependent = dependent;
			internal::MarkAsReady(handle.promise().task_id);
		}

		T Value() const noexcept {
			return **value;
		}
	};

	template<typename TT = T>
	requires(std::same_as<TT, T>)
	AwaitableTaskValue ToAwaitable() {
		auto awaitable = AwaitableTaskValue{
			.handle = handle_,
			.value = handle_.promise().value,
		};
		auto task_ptr = std::shared_ptr<TaskBase>(new Task<T>(std::move(*this)));
		internal::AddTask(task_ptr);
		return awaitable;
	}

private:
	coro_handle handle_;
};

template<typename T>
template<Awaitable A>
typename Task<T>::template AwaitablePromise<A> Task<T>::promise_type::await_transform(A &&a) {
	return AwaitablePromise<A>{
		.awaitable = std::forward<A>(a),
	};
}

template<typename T>
template<AwaitableValue A>
typename Task<T>::template AwaitableValuePromise<A> Task<T>::promise_type::await_transform(A &&a) {
	return AwaitableValuePromise<A>{
		.awaitable = std::forward<A>(a),
	};
}

template<typename T>
template<typename U>
typename Task<T>::template AwaitableValuePromise<typename Task<U>::AwaitableTaskValue> Task<T>::promise_type::await_transform(Task<U>&& task) {
	return await_transform(task.template ToAwaitable()); // lol
}

}