#include <iostream>

#include "runtime/runtime.hpp"
#include "runtime/prelude.hpp"
#include "runtime/task/prelude.hpp"

Task<int> IntProducerTask() {
	std::cout << "test message #2" << std::endl;
	co_return 42;
}

Task<Void> TestTask() {
	std::cout << "test message #1" << std::endl;
	auto i = co_await IntProducerTask();
	std::cout << "received " << i << std::endl;
	co_return {};
}


int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
	RuntimeBuilder{}
		.Init();

	Spawn(TestTask());

	return blobfish::runtime::Runtime::Instance().Run();
}