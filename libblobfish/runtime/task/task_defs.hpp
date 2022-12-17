#pragma once

#include <cstdlib>

#include "util/named_type.hpp"

namespace blobfish::runtime::task {

using TaskId = util::NamedType<size_t, struct TaskIdtag>;

}