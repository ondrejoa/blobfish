#pragma once
#include "runtime_config.hpp"

namespace blobfish::runtime {

class RuntimeBuilder final {
public:
	RuntimeBuilder();
	void Init();

	RuntimeBuilder& NetIoThreads(size_t count);
	RuntimeBuilder& FileIoThreads(size_t count);
	RuntimeBuilder& ComputeThreads(size_t count);
	RuntimeBuilder& MaxCoroutines(size_t count);

private:
	RuntimeConfig config_;
};

}