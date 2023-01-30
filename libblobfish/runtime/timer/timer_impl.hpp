#pragma once

#include <chrono>

#include "runtime/task/task_defs.hpp"

namespace blobfish::runtime::timer {

class TimerImpl {
public:
	explicit TimerImpl(std::chrono::time_point<std::chrono::steady_clock> deadline);
	~TimerImpl();

	bool Ready() const noexcept;
	void Suspend(task::TaskId task) noexcept;

private:
	std::chrono::time_point<std::chrono::steady_clock> deadline_;
};

}