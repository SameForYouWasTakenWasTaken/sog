/**
 * @file TypeTraits.hpp
 * @brief Declares sog::to_underlying, the engine's C++20 replacement for std::to_underlying
 * (C++23), which converts an enum value to its underlying integer type.
 *
 * std::to_underlying only exists in C++23; since the library targets C++20, this header
 * provides the same behaviour so engine code (GL enum marshalling, bit-mask packing, ...)
 * can stay free of static_cast noise.
 */

#pragma once
#include <type_traits>

namespace sog
{
/**
 * @brief Converts an enum value to its underlying integer type (sog's C++20 stand-in for
 * std::to_underlying).
 *
 * Accepts any enumeration (scoped or unscoped) and produces exactly what static_cast to
 * std::underlying_type_t<TEnum> would. The signature is kept compatible with the C++23
 * standard library function, so migrating to it later is a mechanical change.
 *
 * @tparam TEnum Enumeration type; must satisfy std::is_enum_v.
 * @param value Enumerated value to unwrap.
 * @return The enumerator as its underlying integer type.
 *
 * @code
 * auto mask = sog::to_underlying(gfx::ClearBuffer::Depth); // int == GL_DEPTH_BUFFER_BIT
 * @endcode
 */
template <typename TEnum>
[[nodiscard]] constexpr std::underlying_type_t<TEnum> to_underlying(TEnum value) noexcept
    requires std::is_enum_v<TEnum>
{
    return static_cast<std::underlying_type_t<TEnum>>(value);
}
} // namespace sog
