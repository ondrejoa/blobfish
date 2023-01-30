#pragma once

#include <memory>
#include <chrono>
#include <functional>

namespace blobfish::runtime::timer {

class TimerServiceImpl;

class TimerService final {
public:
	TimerService();
	~TimerService();

	struct Handle {}; // TODO

	Handle ScheduleAt(std::chrono::time_point<std::chrono::steady_clock> tp, std::function<void()> resume);

private:
	std::unique_ptr<TimerServiceImpl> pimpl_;
};

}
