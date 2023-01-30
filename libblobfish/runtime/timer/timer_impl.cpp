#include "timer_impl.hpp"

#include "runtime/runtime.hpp"

namespace blobfish::runtime::timer {

TimerImpl::TimerImpl(std::chrono::time_point<std::chrono::steady_clock> deadline): deadline_{deadline} {
}

TimerImpl::~TimerImpl() {}

bool TimerImpl::Ready() const noexcept {
	return deadline_ <= std::chrono::steady_clock::now();
}

void TimerImpl::Suspend(task::TaskId task) noexcept {
	blobfish::runtime::Runtime::Instance().GetTimerService().ScheduleAt(
		deadline_,
		[task] () {
			blobfish::runtime::Runtime::Instance().MarkAsReady(task);
		}
	);
}

}