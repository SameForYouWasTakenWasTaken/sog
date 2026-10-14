/**
 * @file Handles.hpp
 * @brief Defines the engine's typed OpenGL object handles (vertex arrays, buffers, shaders,
 * textures, framebuffers, renderbuffers).
 *
 * Each handle is a GLHandle<Tag> specialized with a distinct empty tag type, so
 * a TextureHandle cannot be passed where a ShaderHandle is expected even though
 * both wrap a GLuint.
 */

#pragma once
#include "../GLHandle.hpp"
#include "../Indices.hpp"
#include "../Vertex.hpp"

namespace sog::gfx
{
// ======== PHANTOM TAG TYPES (identity markers for GLHandle) ======== //

struct VertexBufferTag
{
}; /**< Phantom tag marking a GLHandle as a vertex buffer name. */
struct VertexArrayTag
{
}; /**< Phantom tag marking a GLHandle as a vertex array object name. */
struct ElementBufferTag
{
}; /**< Phantom tag marking a GLHandle as an element (index) buffer name. */
struct ShaderTag
{
}; /**< Phantom tag marking a GLHandle as a shader/program name. */

struct TextureTag
{
}; /**< Phantom tag marking a GLHandle as a texture name. */
struct BufferTag
{
}; /**< Phantom tag marking a GLHandle as a generic buffer name (e.g. uniform/storage buffers). */
struct FrameBufferTag
{
}; /**< Phantom tag marking a GLHandle as a framebuffer name. */
struct RenderBufferTag
{
}; /**< Phantom tag marking a GLHandle as a renderbuffer name. */

struct ShaderProgramTag
{
}; /**< Phantom tag marking a GLHandle as a shader program name. */

// ======== CONCRETE HANDLE TYPES ======== //

/// Handle to a vertex array object (tagged with VertexArrayTag).
using VertexArrayHandle = GLHandle<VertexArrayTag>;
/// Handle to a vertex buffer, tagged with the Vertex layout type it will hold (GLHandle<Vertex>).
using VertexBufferHandle = GLHandle<Vertex>;
/// Handle to an element (index) buffer, tagged with the Index element type it holds
/// (GLHandle<Index>).
using ElementBufferHandle = GLHandle<Index>;

/// Handle to a uniform buffer object, tagged by the element type T it stores.
template <typename T> using UniformBufferHandle = GLHandle<T>;
/// Handle to a shader storage buffer object, tagged by the element type T it stores.
template <typename T> using StorageBufferHandle = GLHandle<T>;

/// Handle to a compiled shader program (tagged with ShaderTag).
using ShaderHandle = GLHandle<ShaderTag>;
/// Handle to a compiled shader program (tagged with ShaderProgramTag).
using ShaderProgramHandle = GLHandle<ShaderProgramTag>;
/// Handle to a texture object (tagged with TextureTag).
using TextureHandle = GLHandle<TextureTag>;
/// Handle to a framebuffer object (tagged with FrameBufferTag).
using FrameBufferHandle = GLHandle<FrameBufferTag>;
/// Handle to a renderbuffer object (tagged with RenderBufferTag).
using RenderBufferHandle = GLHandle<RenderBufferTag>;

} // namespace sog::gfx
