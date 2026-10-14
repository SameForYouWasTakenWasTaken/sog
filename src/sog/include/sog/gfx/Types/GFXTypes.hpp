/**
 * @file GFXTypes.hpp
 * @brief Type aliases mapping engine graphics vocabulary onto raw OpenGL typedefs (handles,
 * vertex-attribute parameters, buffer sizes).
 *
 * These aliases let engine code speak in domain terms (attribute_index,
 * buffer_size) while remaining ABI-identical to the underlying GL types.
 */

#pragma once
#include "glad/gl.h"

namespace sog::gfx
{
// ========= OPENGL OBJECTS ========== //
/// Raw OpenGL object name (GLuint); the primitive stored inside every GLHandle.
using gl_handle = GLuint;

// ========= VERTEX ARRAYS ========== //
/// Location of a vertex attribute within a shader program ( glVertexAttribPointer index ).
using attribute_index = GLuint;
/// Number of components per attribute, 1-4 (e.g. 3 for a vec3 position).
using attribute_size = GLint;
/// Whether integer attribute data is normalized to [0, 1] when read by the shader
/// (GL_TRUE/GL_FALSE).
using attribute_normalized = GLboolean;
/// Byte distance between consecutive attribute elements in the vertex buffer.
using attribute_stride = GLsizei;
/// Raw pointer used as the data source for client-side vertex arrays.
using attribute_pointer = void*;
/// Byte offset of the first attribute element within the bound vertex buffer.
using attribute_offset = GLuint;
/// Vertex buffer binding point an attribute reads from.
using attribute_binding = GLuint;

/**
 * @brief Component type of a vertex attribute, as stored in the vertex buffer.
 */
enum class attribute_type
{
    FLOAT32 = GL_FLOAT, /**< 32-bit floating-point component (GL_FLOAT). */
};

// ========= VERTEX BUFFERS ========== //
/// Size of buffer data in bytes (GLsizeiptr).
using buffer_size = GLsizeiptr;
/// Generic pointer to raw buffer payload data.
using buffer_data = void*;

} // namespace sog::gfx
