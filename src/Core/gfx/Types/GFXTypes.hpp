#pragma once
#include "glad/gl.h"

namespace sog::gfx
{
// ========= OPENGL OBJECTS ========== //
using gl_handle = GLuint;
// ========= VERTEX ARRAYS ========== //
using attribute_index = GLuint;
using attribute_size = GLint;
using attribute_normalized = GLboolean;
using attribute_stride = GLsizei;
using attribute_pointer = void*;
using attribute_offset = GLuint;
using attribute_binding = GLuint;

enum class attribute_type
{
    FLOAT32 = GL_FLOAT,
};

// ========= VERTEX BUFFERS ========== //
using buffer_size = GLsizeiptr;
using buffer_data = void*;

} // namespace sog::gfx