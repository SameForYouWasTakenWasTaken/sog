#pragma once
#include "glad/gl.h"

namespace sog::gfx
{

enum class error
{
    NONE = GL_NONE
};

enum class buffer_target : GLenum
{
    VBO_BUFFER = GL_ARRAY_BUFFER,
    EBO_BUFFER = GL_ELEMENT_ARRAY_BUFFER,
};
enum class buffer_query : GLenum
{
    BINDING = GL_ARRAY_BUFFER_BINDING,
};

enum class buffer_usage : GLenum
{
    STREAM_DRAW = GL_STREAM_DRAW,
    STREAM_READ = GL_STREAM_READ,
    STREAM_COPY = GL_STREAM_COPY,

    DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
    DYNAMIC_COPY = GL_DYNAMIC_COPY,
    DYNAMIC_READ = GL_DYNAMIC_READ,

    STATIC_DRAW = GL_STATIC_DRAW,
    STATIC_COPY = GL_STATIC_COPY,
    STATIC_READ = GL_STATIC_READ,
};
enum class vertex_target : GLenum
{
    VAO_BUFFER = GL_ARRAY_BUFFER,
};
enum class vertex_query : GLenum
{
    BINDING = GL_VERTEX_ARRAY_BINDING,
};

enum class shader_type : GLenum
{
    VERTEX = GL_VERTEX_SHADER,
    FRAGMENT = GL_FRAGMENT_SHADER,
    GEOMETRY = GL_GEOMETRY_SHADER,
    COMPUTE = GL_COMPUTE_SHADER,
};

enum class texture_params : GLint
{
    MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
    REPEAT = GL_REPEAT,
    CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
    CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
};

enum class texture_min_filters : GLint
{
    NEAREST = GL_NEAREST,
    LINEAR = GL_LINEAR,
    NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
    LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
    NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
    LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
};

enum class texture_mag_filters : GLint
{
    NEAREST = GL_NEAREST,
    LINEAR = GL_LINEAR,
};

enum class texture_mipmaps : GLenum
{
    NEAREST = GL_NEAREST_MIPMAP_NEAREST,
    LINEAR_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
    NEAREST_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
    LINEAR_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
};
} // namespace sog::gfx