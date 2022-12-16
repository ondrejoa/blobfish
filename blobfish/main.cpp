#include "runtime/runtime.hpp"
#include "runtime/prelude.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
	RuntimeBuilder{}
		.Init();

	return blobfish::runtime::Runtime::Instance().Run();
}