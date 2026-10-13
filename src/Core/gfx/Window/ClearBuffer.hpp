#pragma once
#include <utility>

#include "glad/gl.h"
namespace sog::gfx
{
enum class ClearBuffer : unsigned int
{
    Color = GL_COLOR_BUFFER_BIT,
    Depth = GL_DEPTH_BUFFER_BIT,
    Stencil = GL_STENCIL_BUFFER_BIT
};
constexpr ClearBuffer operator|(ClearBuffer lhs, ClearBuffer rhs) noexcept
{
    return static_cast<ClearBuffer>(std::to_underlying(lhs) | std::to_underlying(rhs));
}
} // namespace sog::gfx