#pragma once

#include <concepts>

#include "task_defs.hpp"

namespace blobfish::runtime::task {

template<typename A>
concept ProducesValue = requires {
	typename A::ValueType;
};

template<typename A>
concept AwaitableBase = std::move_constructible<A> && requires(const A a) {
	{ a.Ready() } noexcept -> std::same_as<bool>;
} && requires(A a, TaskId t) {
	{ a.Suspend(t) } noexcept -> std::same_as<void>;
};

template<typename A>
concept Awaitable = AwaitableBase<A> && !ProducesValue<A>;

template<typename A>
concept AwaitableValue = AwaitableBase<A> && ProducesValue<A> && requires(const A a) {
	{ a.Value() } noexcept -> std::same_as<typename A::ValueType>;
};

}