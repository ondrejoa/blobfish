#include "timer_service.hpp"

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <compare>
#include <utility>

using namespace std::chrono_literals;
using time_point = std::chrono::time_point<std::chrono::steady_clock>;

namespace blobfish::runtime::timer {

struct TimerItem {
	std::chrono::time_point<std::chrono::steady_clock> tp;
	std::function<void()> resume;
};

std::strong_ordering operator<=> (const TimerItem& lhs, const TimerItem& rhs) {
	return lhs.tp <=> rhs.tp;
}


class TimerServiceImpl {
public:
	TimerServiceImpl();
	TimerService::Handle ScheduleAt(time_point tp, std::function<void()> resume);

private:
	[[noreturn]] void Run();

	std::priority_queue<TimerItem, std::vector<TimerItem>, std::greater<>> queue_;
	std::mutex mutex_;
	std::condition_variable cond_;
};

TimerService::TimerService(): pimpl_{new TimerServiceImpl} {
}

TimerService::~TimerService() {

}
TimerService::Handle TimerService::ScheduleAt(time_point tp, std::function<void()> resume) {
	pimpl_->ScheduleAt(tp, std::move(resume));
}

TimerServiceImpl::TimerServiceImpl() {
	std::thread {
		[this] () {
			this->Run();
		}
	}.detach();
}

void TimerServiceImpl::Run() {
	while (true) {
		time_point deadline = std::chrono::steady_clock::now() + 60'000ms;
		{
			std::lock_guard lock {mutex_};
			if (!queue_.empty()) {
				deadline = queue_.top().tp;
			}
		}
		std::unique_lock lock {mutex_};
		cond_.wait_until(lock, deadline);
		time_point now = std::chrono::steady_clock::now();
		while (!queue_.empty() && queue_.top().tp <= now) {
			queue_.top().resume();
			queue_.pop();
		}
	}
}

TimerService::Handle TimerServiceImpl::ScheduleAt(time_point tp, std::function<void()> resume) {
	{
		std::scoped_lock lock {mutex_};
		queue_.emplace(
			TimerItem {
				.tp = tp,
				.resume = std::move(resume)
			}
		);
	}
	cond_.notify_one();
	return {};
}

}
