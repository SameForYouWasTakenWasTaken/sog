/**
 * @file RenderState.hpp
 * @brief Defines RenderState, a bundle of the GPU objects needed to issue one draw call (buffers,
 * vertex array, optional texture, shader program).
 */

#pragma once
#include <optional>
#include <sog/gfx/OpenGL/Objects/ShaderProgram.hpp>
#include <sog/gfx/OpenGL/Objects/Texture2D.hpp>
#include <sog/gfx/OpenGL/Objects/VertexArray.hpp>

namespace sog::gfx
{
/**
 * @brief Everything Window::draw() needs to render one batch.
 *
 * Holds references (not owning) to the vertex buffer and vertex array backing the
 * geometry, an optional texture, and the linked shader program to use. Pass an
 * instance to Window::draw(), which binds the program and vertex array, optionally
 * binds the texture to unit 0, and issues either glDrawElements or glDrawArrays
 * depending on whether the vertex array has an index buffer.
 *
 * @attention All reference members must outlive the RenderState and the draw call;
 * this struct never extends their lifetime. texture2D may be null for untextured draws.
 *
 * @see sog::Window::draw
 */
struct RenderState
{
    const VertexBuffer& vertexBuffer; /**< Geometry data backing the draw; used for the vertex count
                                         in non-indexed draws. */
    const VertexArray& vertexArray;   /**< Vertex array object defining the attribute layout and,
                                         optionally, the index buffer. */
    const Texture2D* texture2D =
        nullptr; /**< Optional texture bound to unit 0; null means no texture is bound. */

    const ShaderProgram&
        shaderProgram; /**< Linked program whose uniforms/attributes the draw uses. */
};
} // namespace sog::gfx
