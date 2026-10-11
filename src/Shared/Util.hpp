#pragma once
#include <type_traits>

namespace sog::util
{
template <typename E> constexpr auto to_underlying(E value) noexcept
{
    return static_cast<std::underlying_type_t<E>>(value);
}
} // namespace sog::util