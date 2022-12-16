#include "runtime_builder.hpp"

#include <thread>

#include "runtime.hpp"

namespace blobfish::runtime {

RuntimeConfig DefaultConfig() {
	return RuntimeConfig {
		.net_io_threads = std::thread::hardware_concurrency(),
		.file_io_threads = 1,
		.compute_threads = std::thread::hardware_concurrency(),
		.max_coroutines = 64,
	};
}

RuntimeBuilder::RuntimeBuilder(): config_{DefaultConfig()} {
}

void RuntimeBuilder::Init() {
	Runtime::Instance().InitRuntime(config_);
}

RuntimeBuilder& RuntimeBuilder::NetIoThreads(size_t count) {
	config_.net_io_threads = count;
	return *this;
}

RuntimeBuilder& RuntimeBuilder::FileIoThreads(size_t count) {
	config_.file_io_threads = count;
	return *this;
}

RuntimeBuilder& RuntimeBuilder::ComputeThreads(size_t count) {
	config_.compute_threads = count;
	return *this;
}

RuntimeBuilder& RuntimeBuilder::MaxCoroutines(size_t count) {
	config_.max_coroutines = count;
	return *this;
}

}

