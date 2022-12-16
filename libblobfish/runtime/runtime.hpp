#pragma once

#include <memory>

#include "runtime_config.hpp"

namespace blobfish::runtime {

class RuntimeImpl;

class Runtime final {
	friend class RuntimeBuilder;
public:
	~Runtime();

	int Run();

	static Runtime& Instance();
private:
	Runtime();
	void InitRuntime([[maybe_unused]] RuntimeConfig config);

	std::unique_ptr<RuntimeImpl> pimpl_;
};

}