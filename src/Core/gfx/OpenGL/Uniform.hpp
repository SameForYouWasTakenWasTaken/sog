#pragma once
#include "glad/gl.h"
namespace sog::gfx
{
struct Uniform
{
    using Location = GLint;
    Location location = -1;

    [[nodiscard]] constexpr bool valid() const noexcept { return location != -1; }
    [[nodiscard]] constexpr explicit operator bool() const noexcept { return valid(); }
    [[nodiscard]] constexpr explicit operator Location() const noexcept { return location; }
};
} // namespace sog::gfx