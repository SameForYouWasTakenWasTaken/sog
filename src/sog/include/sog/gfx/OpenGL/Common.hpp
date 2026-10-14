/**
 * @file Common.hpp
 * @brief Declares the sog::gfx::common draw entry points: thin type-safe wrappers around
 * glDrawArrays/glDrawElements (and their instanced variants) plus the viewport helper.
 *
 * Taking the engine's draw_mode/index_type enums instead of raw GLenums keeps
 * call sites free of GL constants; the wrappers forward unchanged otherwise.
 */

#pragma once
#include "../Types/Enums.hpp"
#include "glad/gl.h"

namespace sog::gfx::common
{
/**
 * @brief Draws a range of consecutive vertices (glDrawArrays).
 * @param mode Primitive topology to assemble the vertices into.
 * @param first Index of the first vertex to read from the bound vertex buffer.
 * @param count Number of vertices to draw.
 */
void DrawArrays(draw_mode mode, GLint first, GLsizei count);

/**
 * @brief Draws vertices through an element (index) buffer (glDrawElements).
 * @param mode Primitive topology to assemble the indices into.
 * @param count Number of indices to read.
 * @param type Component type of the indices in the bound element buffer.
 * @param indices Byte offset of the first index within the bound element buffer; nullptr starts at
 * the beginning (the usual case with a VAO-managed index buffer).
 */
void DrawElements(draw_mode mode, GLsizei count, index_type type, const void* indices = nullptr);

/**
 * @brief Draws a range of vertices repeatedly, once per instance (glDrawArraysInstanced).
 * @param mode Primitive topology to assemble the vertices into.
 * @param first Index of the first vertex to read from the bound vertex buffer.
 * @param count Number of vertices per instance.
 * @param instanceCount Number of instances to draw.
 */
void DrawArraysInstanced(draw_mode mode, GLint first, GLsizei count, GLsizei instanceCount);

/**
 * @brief Draws indexed geometry repeatedly, once per instance (glDrawElementsInstanced).
 * @param mode Primitive topology to assemble the indices into.
 * @param count Number of indices per instance.
 * @param type Component type of the indices in the bound element buffer.
 * @param indices Byte offset of the first index within the bound element buffer.
 * @param instanceCount Number of instances to draw.
 */
void DrawElementsInstanced(draw_mode mode, GLsizei count, index_type type, const void* indices,
                           GLsizei instanceCount);

/**
 * @brief Sets the viewport rectangle of the currently active render target (glViewport).
 * @param x Left edge of the viewport in pixels (origin at bottom-left).
 * @param y Bottom edge of the viewport in pixels.
 * @param width Viewport width in pixels.
 * @param height Viewport height in pixels.
 *
 * Called with the framebuffer's size on resize; see the Window resize callback.
 */
void ViewportToActiveTarget(int x, int y, int width, int height);
} // namespace sog::gfx::common
