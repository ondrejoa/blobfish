#pragma once

#include <compare>

namespace blobfish::util {

template<typename UNDERLYING_TYPE, typename TAG>
struct NamedType {
	using Type = UNDERLYING_TYPE;
	using Tag [[maybe_unused]] = TAG;

	Type value;

	explicit operator Type&() {
		return value;
	}

	explicit operator const Type&() const {
		return value;
	}

	NamedType(const Type& v): value{v} {} // NOLINT(google-explicit-constructor)

	NamedType& operator=(const Type& v) {
		this->value = v;
		return *this;
	}

	NamedType(const NamedType&) = default;

	NamedType &operator=(const NamedType&) = default;

	// TODO constrain
	std::strong_ordering operator<=>(const NamedType& rhs) const {
		return value <=> rhs.value;
	}
};

}