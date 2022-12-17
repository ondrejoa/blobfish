#include <iostream>

#include "runtime/runtime.hpp"
#include "runtime/prelude.hpp"
#include "runtime/task/prelude.hpp"

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
	std::cout << "dummy value " << value << std::endl;
	co_return 42;
}

Task<Void> TestTask() {
	std::cout << "test message #1" << std::endl;
	auto i = co_await IntProducerTask().ToAwaitable();
	std::cout << "received " << i << std::endl;
	co_return {};
}


int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
	RuntimeBuilder{}
		.Init();

	Spawn(TestTask());

	return blobfish::runtime::Runtime::Instance().Run();
}