#include "runtime.hpp"

namespace blobfish::runtime {

class RuntimeImpl {
public:
private:
};

Runtime::Runtime() {}

Runtime::~Runtime() {}

Runtime &Runtime::Instance() {
	static Runtime runtime;
	return runtime;
}

void Runtime::InitRuntime([[maybe_unused]] RuntimeConfig runtime) {
	// TODO
	pimpl_ = std::make_unique<RuntimeImpl>();
}

int Runtime::Run() {
	return 0;
}

}