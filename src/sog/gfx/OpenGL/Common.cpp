/**
 * @file Common.cpp
 * @brief Implements the sog::gfx::common draw entry points as direct forwards to their GL
 * counterparts (to_underlying converts the engine enums on the way out).
 */

#include <sog/Utility/TypeTraits.hpp>
#include <sog/gfx/OpenGL/Common.hpp>

namespace sog::gfx
{

void common::DrawArrays(draw_mode mode, GLint first, GLsizei count)
{
    glDrawArrays(to_underlying(mode), first, count);
}
void common::DrawElements(draw_mode mode, GLsizei count, index_type type, const void* indices)
{
    glDrawElements(to_underlying(mode), count, to_underlying(type), indices);
}
void common::DrawArraysInstanced(draw_mode mode, GLint first, GLsizei count, GLsizei instanceCount)
{
    glDrawArraysInstanced(to_underlying(mode), first, count, instanceCount);
}
void common::DrawElementsInstanced(draw_mode mode, GLsizei count, index_type type,
                                   const void* indices, GLsizei instanceCount)
{
    glDrawElementsInstanced(to_underlying(mode), count, to_underlying(type), indices,
                            instanceCount);
}
void common::ViewportToActiveTarget(int x, int y, int width, int height)
{
    glViewport(x, y, width, height);
}
} // namespace sog::gfx