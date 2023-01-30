#pragma once

#include <memory>
#include <chrono>

#include "runtime/task/task_defs.hpp"
#include "runtime/task/awaitable.hpp"

namespace blobfish::runtime::timer {

class TimerImpl;

class Timer {
public:
	explicit Timer(std::chrono::milliseconds millis);
	explicit Timer(std::chrono::time_point<std::chrono::steady_clock> deadline);
	~Timer();

	Timer(Timer&&);

	bool Ready() const noexcept;
	void Suspend(task::TaskId task) noexcept;

private:
	std::unique_ptr<TimerImpl> pimpl_;
};

static_assert(task::Awaitable<Timer>);

}