/**
 * @file ClearBuffer.hpp
 * @brief Defines the ClearBuffer enum (color/depth/stencil bits) and a bitwise-or combinator for
 * clearing multiple buffers at once.
 */

#pragma once
#include "../../Utility/TypeTraits.hpp"

#include "glad/gl.h"
namespace sog::gfx
{
/**
 * @brief Which framebuffer buffer(s) to clear, mirroring the GL_COLOR_BUFFER_BIT /
 * GL_DEPTH_BUFFER_BIT / GL_STENCIL_BUFFER_BIT flags.
 *
 * Values are the raw GL bit masks, so a combination can be passed straight to
 * glClear. Combine members with operator| (e.g. ClearBuffer::Color | ClearBuffer::Depth).
 */
enum class ClearBuffer : unsigned int
{
    Color = GL_COLOR_BUFFER_BIT,    /**< Clear the color buffer. */
    Depth = GL_DEPTH_BUFFER_BIT,    /**< Clear the depth buffer. */
    Stencil = GL_STENCIL_BUFFER_BIT /**< Clear the stencil buffer. */
};

/**
 * @brief Bitwise-or of two ClearBuffer flags, producing a combined mask.
 * @param lhs First buffer flag.
 * @param rhs Second buffer flag.
 * @return ClearBuffer whose underlying value is the bitwise-or of both inputs.
 *
 * @code
 * window.beginFrame(ClearBuffer::Color | ClearBuffer::Depth);
 * @endcode
 */
constexpr ClearBuffer operator|(ClearBuffer lhs, ClearBuffer rhs) noexcept
{
    return static_cast<ClearBuffer>(to_underlying(lhs) | to_underlying(rhs));
}
} // namespace sog::gfx
