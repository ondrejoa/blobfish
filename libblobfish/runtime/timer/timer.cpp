#include "timer.hpp"

#include "timer_impl.hpp"

namespace blobfish::runtime::timer {

Timer::Timer(std::chrono::milliseconds millis): Timer{std::chrono::steady_clock::now() + millis} {

}

Timer::Timer(std::chrono::time_point<std::chrono::steady_clock> deadline): pimpl_{new TimerImpl{deadline}} {
}

Timer::~Timer() {}

Timer::Timer(Timer&& timer): pimpl_{std::move(timer.pimpl_)} {}

bool Timer::Ready() const noexcept {
	return pimpl_->Ready();
}

void Timer::Suspend(task::TaskId task) noexcept {
	pimpl_->Suspend(task);
}

}