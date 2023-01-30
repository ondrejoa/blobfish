#include <iostream>

#include <chrono>

#include "runtime/runtime.hpp"
#include "runtime/prelude.hpp"
#include "runtime/task/prelude.hpp"
#include "runtime/timer/prelude.hpp"

using namespace std::chrono_literals;

struct DummyAwaitable {
	bool Ready() const noexcept {
		std::cout << "DUMMY.Ready()" << std::endl;
		return false;
	}

	void Suspend(blobfish::runtime::task::TaskId id) noexcept {
		std::cout << "DUMMY.Suspend()" << std::endl;
		blobfish::runtime::Runtime::Instance().MarkAsReady(id);
	}
};

static_assert(blobfish::runtime::task::Awaitable<DummyAwaitable>);

struct DummyAwaitableValue {
	using ValueType = int;

	bool Ready() const noexcept {
		std::cout << "DUMMY_VALUE.Ready()" << std::endl;
		return false;
	}

	void Suspend(blobfish::runtime::task::TaskId id) noexcept {
		std::cout << "DUMMY.Suspend()" << std::endl;
		blobfish::runtime::Runtime::Instance().MarkAsReady(id);
	}

	int Value() const noexcept {
		return 666;
	}

};

static_assert(!blobfish::runtime::task::Awaitable<DummyAwaitableValue>);
static_assert(blobfish::runtime::task::AwaitableValue<DummyAwaitableValue>);

Task<int> IntProducerTask() {
	co_await DummyAwaitable{};
	std::cout << "test message #2" << std::endl;
	int value = co_await DummyAwaitableValue{};
	std::cout << "Waiting 5s" << std::endl;
	co_await Timer{5'000ms};
	std::cout << "dummy value " << value << std::endl;
	co_return 42;
}

Task<> TestTask() {
	std::cout << "test message #1" << std::endl;
	auto i = co_await IntProducerTask();
	std::cout << "received " << i << std::endl;
	co_return {};
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
	RuntimeBuilder{}
		.Init();

	Spawn(TestTask());

	return blobfish::runtime::Runtime::Instance().Run();
}