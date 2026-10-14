#pragma once
#include "../Types/Enums.hpp"
#include "glad/gl.h"

namespace sog::gfx::common
{
void DrawArrays(draw_mode mode, GLint first, GLsizei count);
void DrawElements(draw_mode mode, GLsizei count, index_type type, const void* indices = nullptr);
void DrawArraysInstanced(draw_mode mode, GLint first, GLsizei count, GLsizei instanceCount);
void DrawElementsInstanced(draw_mode mode, GLsizei count, index_type type, const void* indices,
                           GLsizei instanceCount);

void ViewportToActiveTarget(int x, int y, int width, int height);
} // namespace sog::gfx::common