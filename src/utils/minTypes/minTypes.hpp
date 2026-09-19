#pragma once
#include <cstdint>
#include <limits>
#include <type_traits>

template <std::size_t maxVal>
using minUint_t = std::conditional_t<(maxVal <= std::numeric_limits<std::uint8_t>::max()), std::uint8_t,
    std::conditional_t<(maxVal <= std::numeric_limits<std::uint16_t>::max()), std::uint16_t,
    std::conditional_t<(maxVal <= std::numeric_limits<std::uint32_t>::max()), std::uint32_t,
    std::uint64_t>>>;

template <std::size_t val>
constexpr auto minUint() {
    return static_cast<minUint_t<val>>(val);
}