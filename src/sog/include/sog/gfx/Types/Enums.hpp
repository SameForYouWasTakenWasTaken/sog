/**
 * @file Enums.hpp
 * @brief Type-safe enums over raw OpenGL constants (buffers, vertex arrays, shaders, textures) for
 * use throughout the engine.
 */

#pragma once
#include "glad/gl.h"

namespace sog::gfx
{
inline namespace enums
{
/**
 * @brief Sentinel for "no error" (mirrors GL_NONE).
 */
enum class error
{
    NONE = GL_NONE /**< No error pending. */
};

/**
 * @brief Target a buffer object can be bound to (mirrors GL_ARRAY_BUFFER /
 * GL_ELEMENT_ARRAY_BUFFER).
 * @see buffer_usage
 */
enum class buffer_target : GLenum
{
    VBO_BUFFER = GL_ARRAY_BUFFER, /**< Vertex buffer target (GL_ARRAY_BUFFER). */
    EBO_BUFFER =
        GL_ELEMENT_ARRAY_BUFFER, /**< Element (index) buffer target (GL_ELEMENT_ARRAY_BUFFER). */
};

/**
 * @brief State to query for a bound vertex buffer (mirrors GL_ARRAY_BUFFER_BINDING).
 */
enum class buffer_query : GLenum
{
    BINDING = GL_ARRAY_BUFFER_BINDING, /**< Query the buffer currently bound to GL_ARRAY_BUFFER. */
};

/**
 * @brief Expected usage pattern of a buffer, guiding GL driver storage (mirrors
 * GL_*_DRAW/READ/COPY).
 *
 * Hints only — they do not change correctness. STREAM means the data changes once
 * and is used a few times, DYNAMIC a few times, STATIC many times; DRAW consumes
 * the data for rendering, READ is read back by the client, COPY is reused as
 * source data for other GL commands.
 */
enum class buffer_usage : GLenum
{
    STREAM_DRAW = GL_STREAM_DRAW, /**< Modified once, consumed a few times for drawing. */
    STREAM_READ = GL_STREAM_READ, /**< Modified once, then read back a few times. */
    STREAM_COPY = GL_STREAM_COPY, /**< Modified once, then used as source data a few times. */

    DYNAMIC_DRAW =
        GL_DYNAMIC_DRAW, /**< Modified a handful of times, consumed many times for drawing. */
    DYNAMIC_COPY =
        GL_DYNAMIC_COPY, /**< Modified a handful of times, then used as source data repeatedly. */
    DYNAMIC_READ = GL_DYNAMIC_READ, /**< Modified a handful of times, then read back repeatedly. */

    STATIC_DRAW =
        GL_STATIC_DRAW, /**< Modified once at creation, consumed many times for drawing. */
    STATIC_COPY =
        GL_STATIC_COPY, /**< Modified once at creation, then used as source data repeatedly. */
    STATIC_READ = GL_STATIC_READ, /**< Modified once at creation, then read back repeatedly. */
};

/**
 * @brief Target a vertex array object can be bound to (mirrors GL_ARRAY_BUFFER).
 */
enum class vertex_target : GLenum
{
    VAO_BUFFER = GL_ARRAY_BUFFER, /**< Buffer target associated with vertex attribute setup. */
};

/**
 * @brief State to query for a bound vertex array object (mirrors GL_VERTEX_ARRAY_BINDING).
 */
enum class vertex_query : GLenum
{
    BINDING = GL_VERTEX_ARRAY_BINDING, /**< Query the vertex array currently bound to the active
                                          target. */
};

/**
 * @brief Shader stage types (mirrors GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER,
 * GL_COMPUTE_SHADER).
 * @see sog::backend::Shader, sog::backend::ShaderProgram
 */
enum class shader_type : GLenum
{
    VERTEX = GL_VERTEX_SHADER,     /**< Vertex processing stage. */
    FRAGMENT = GL_FRAGMENT_SHADER, /**< Fragment (per-pixel) processing stage. */
    GEOMETRY =
        GL_GEOMETRY_SHADER,      /**< Geometry processing stage (requires GL 3.2+ or extensions). */
    COMPUTE = GL_COMPUTE_SHADER, /**< Compute stage (requires GL 4.3+). */
};

/**
 * @brief Texture wrap modes, applied when texture coordinates leave [0, 1] (mirrors
 * GL_MIRRORED_REPEAT, GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER).
 */
enum class texture_params : GLint
{
    MIRRORED_REPEAT = GL_MIRRORED_REPEAT, /**< Wraps and mirrors at each repeat. */
    REPEAT = GL_REPEAT,               /**< Wraps and repeats, resetting at each integer boundary. */
    CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE, /**< Clamps to the edge texel of the texture. */
    CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER, /**< Clamps to a user-defined border color. */
};

/**
 * @brief Minification filters for textures, including mipmap-aware variants (mirrors GL_NEAREST,
 * GL_LINEAR and the GL_*_MIPMAP_* combinations).
 *
 * The mipmapped entries select between two mip levels by distance (LINEAR) or
 * pick the nearest level and filter within it (NEAREST).
 */
enum class texture_min_filters : GLint
{
    NEAREST = GL_NEAREST, /**< Closest texel; blocky when magnified/minified. */
    LINEAR = GL_LINEAR,   /**< Bilinear interpolation between texels. */
    NEAREST_MIPMAP_NEAREST =
        GL_NEAREST_MIPMAP_NEAREST, /**< Nearest mip level, nearest texel within it. */
    LINEAR_MIPMAP_NEAREST =
        GL_LINEAR_MIPMAP_NEAREST, /**< Nearest mip level, linear filtering within it. */
    NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR, /**< Linear blend between the two nearest mip
                                                         levels, nearest texel. */
    LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,   /**< Trilinear filtering: linear across mip
                                                         levels and within each. */
};

/**
 * @brief Magnification filters for textures (mirrors GL_NEAREST, GL_LINEAR; no mipmap variants
 * exist for magnification).
 */
enum class texture_mag_filters : GLint
{
    NEAREST = GL_NEAREST, /**< Closest texel; crisp pixel-art look. */
    LINEAR = GL_LINEAR,   /**< Bilinear interpolation; smooth look. */
};

/**
 * @brief Mipmap level-selection strategies derived from the minification filter (mirrors the
 * GL_*_MIPMAP_* constants).
 *
 * Names follow the "level filter, within-level filter" convention:
 * LINEAR_NEAREST blends linearly between mip levels but samples the nearest
 * texel inside each level, and so on.
 */
enum class texture_mipmaps : GLenum
{
    NEAREST = GL_NEAREST_MIPMAP_NEAREST, /**< Nearest level, nearest texel within it. */
    LINEAR_NEAREST =
        GL_LINEAR_MIPMAP_NEAREST, /**< Nearest level, linear texel filtering within it. */
    NEAREST_LINEAR =
        GL_NEAREST_MIPMAP_LINEAR, /**< Linear level blend, nearest texel within levels. */
    LINEAR_LINEAR =
        GL_LINEAR_MIPMAP_LINEAR, /**< Trilinear: linear level blend and linear texel filtering. */
};

enum class texture_internal_format : GLenum
{
    RGBA8 = GL_RGBA8,
    RGB8 = GL_RGB8,
    RGBA16F = GL_RGBA16F,
    RGB16F = GL_RGB16F,
    R8 = GL_R8,
    Depth24Stencil8 = GL_DEPTH24_STENCIL8
};

enum class pixel_format : GLenum
{
    RGBA = GL_RGBA,
    RGB = GL_RGB,
    Red = GL_RED,
    RG = GL_RG,
    DepthStencil = GL_DEPTH_STENCIL
};

enum class pixel_type : GLenum
{
    UnsignedByte = GL_UNSIGNED_BYTE,
    Float = GL_FLOAT,
    Int = GL_INT,
    UnsignedInt248 = GL_UNSIGNED_INT_24_8
};

enum class draw_mode : GLenum
{
    TRIANGLES = GL_TRIANGLES,
    QUADS = GL_QUADS,
    POINTS = GL_POINTS,
    LINES = GL_LINES,
    LINE_STRIP = GL_LINE_STRIP,
    TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
    TRIANGLE_FAN = GL_TRIANGLE_FAN,
    LINES_ADJACENCY = GL_LINES_ADJACENCY,
    LINE_STRIP_ADJACENCY = GL_LINE_STRIP_ADJACENCY,
    TRIANGLES_ADJACENCY = GL_TRIANGLES_ADJACENCY,
    TRIANGLE_STRIP_ADJACENCY = GL_TRIANGLE_STRIP_ADJACENCY,
};

enum class index_type : GLenum
{
    UBYTE = GL_UNSIGNED_BYTE,
    USHORT = GL_UNSIGNED_SHORT,
    UINT = GL_UNSIGNED_INT
};
} // namespace enums
} // namespace sog::gfx
