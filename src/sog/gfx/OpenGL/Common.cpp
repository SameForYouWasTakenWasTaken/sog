#include <sog/gfx/OpenGL/Common.hpp>
#include <utility>

namespace sog::gfx
{

void common::DrawArrays(draw_mode mode, GLint first, GLsizei count)
{
    glDrawArrays(std::to_underlying(mode), first, count);
}
void common::DrawElements(draw_mode mode, GLsizei count, index_type type, const void* indices)
{
    glDrawElements(std::to_underlying(mode), count, std::to_underlying(type), indices);
}
void common::DrawArraysInstanced(draw_mode mode, GLint first, GLsizei count, GLsizei instanceCount)
{
    glDrawArraysInstanced(std::to_underlying(mode), first, count, instanceCount);
}
void common::DrawElementsInstanced(draw_mode mode, GLsizei count, index_type type,
                                   const void* indices, GLsizei instanceCount)
{
    glDrawElementsInstanced(std::to_underlying(mode), count, std::to_underlying(type), indices,
                            instanceCount);
}
void common::ViewportToActiveTarget(int x, int y, int width, int height)
{
    glViewport(x, y, width, height);
}
} // namespace sog::gfx