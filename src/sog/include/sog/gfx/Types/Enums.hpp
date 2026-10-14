/**
 * @file Enums.hpp
 * @brief Type-safe enums over raw OpenGL constants (buffers, vertex arrays, shaders, textures,
 * pixel formats and draw-call parameters) for use throughout the engine.
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
 * @see sog::Shader, sog::ShaderProgram
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

/**
 * @brief Sized GL internal formats a texture storage can be allocated with (mirrors GL_RGBA8,
 * GL_RGB8, GL_RGBA16F, GL_RGB16F, GL_R8, GL_DEPTH24_STENCIL8).
 *
 * Chosen at allocation time by Texture2D::createFromRaw(); it decides how the
 * render target stores texels, independently of the pixel_format of the data
 * uploaded afterwards.
 *
 * @see sog::pixel_format, sog::pixel_type
 */
enum class texture_internal_format : GLenum
{
    RGBA8 = GL_RGBA8,           /**< 8 bits per channel, RGBA (LDR, most common). */
    RGB8 = GL_RGB8,             /**< 8 bits per channel, RGB (no alpha). */
    RGBA16F = GL_RGBA16F,       /**< 16-bit float per channel, RGBA (HDR render targets). */
    RGB16F = GL_RGB16F,         /**< 16-bit float per channel, RGB (HDR, no alpha). */
    R8 = GL_R8,                 /**< Single 8-bit channel (masks, lookup data). */
    Depth24Stencil8 = GL_DEPTH24_STENCIL8 /**< 24-bit depth plus 8-bit stencil. */
};

/**
 * @brief Pixel layout of data uploaded to (or read from) a texture (mirrors GL_RGBA, GL_RGB,
 * GL_RED, GL_RG, GL_DEPTH_STENCIL).
 *
 * Describes the channels present in the caller's pixel data, not how the
 * texture stores them internally.
 *
 * @see sog::texture_internal_format, sog::pixel_type
 */
enum class pixel_format : GLenum
{
    RGBA = GL_RGBA,                 /**< Four channels: red, green, blue, alpha. */
    RGB = GL_RGB,                   /**< Three channels: red, green, blue. */
    Red = GL_RED,                   /**< Single red channel. */
    RG = GL_RG,                     /**< Two channels: red and green. */
    DepthStencil = GL_DEPTH_STENCIL /**< Packed depth/stencil data. */
};

/**
 * @brief Component type of the caller's pixel data (mirrors GL_UNSIGNED_BYTE, GL_FLOAT, GL_INT,
 * GL_UNSIGNED_INT_24_8).
 *
 * Tells OpenGL how to interpret the bytes passed to a texture upload; the
 * default for image files loaded via Texture2D::createFromFile is UnsignedByte.
 *
 * @see sog::texture_internal_format, sog::pixel_format
 */
enum class pixel_type : GLenum
{
    UnsignedByte = GL_UNSIGNED_BYTE,         /**< 8-bit unsigned integer per component. */
    Float = GL_FLOAT,                        /**< 32-bit float per component (HDR data). */
    Int = GL_INT,                            /**< 32-bit signed integer per component. */
    UnsignedInt248 =
        GL_UNSIGNED_INT_24_8 /**< Single packed 24-bit depth + 8-bit stencil value. */
};

/**
 * @brief Primitive topology for a draw call (mirrors GL_TRIANGLES, GL_POINTS, GL_LINES and the
 * strip/fan/adjacency variants).
 *
 * Passed to the sog::gfx::common draw entry points; adjacency variants are only
 * meaningful for geometry shaders.
 *
 * @see sog::gfx::common::DrawArrays, sog::gfx::common::DrawElements
 */
enum class draw_mode : GLenum
{
    TRIANGLES = GL_TRIANGLES,     /**< Separate triangles (3 vertices each). */
    QUADS = GL_QUADS,             /**< Quads (legacy; not in core profile). */
    POINTS = GL_POINTS,           /**< Independent points. */
    LINES = GL_LINES,             /**< Independent line segments (2 vertices each). */
    LINE_STRIP = GL_LINE_STRIP,   /**< Connected line strip. */
    TRIANGLE_STRIP = GL_TRIANGLE_STRIP, /**< Connected triangle strip. */
    TRIANGLE_FAN = GL_TRIANGLE_FAN,     /**< Triangle fan around the first vertex. */
    LINES_ADJACENCY =
        GL_LINES_ADJACENCY, /**< Segments with adjacency vertices (geometry shaders). */
    LINE_STRIP_ADJACENCY =
        GL_LINE_STRIP_ADJACENCY, /**< Line strip with adjacency vertices (geometry shaders). */
    TRIANGLES_ADJACENCY =
        GL_TRIANGLES_ADJACENCY, /**< Triangles with adjacency vertices (geometry shaders). */
    TRIANGLE_STRIP_ADJACENCY =
        GL_TRIANGLE_STRIP_ADJACENCY /**< Strip with adjacency vertices (geometry shaders). */
};

/**
 * @brief Component type of the indices stored in an element (index) buffer (mirrors
 * GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT).
 *
 * Passed to the indexed draw entry points; must match how the index data was
 * uploaded (sog::Index is a 32-bit unsigned integer, i.e. index_type::UINT).
 *
 * @see sog::Index, sog::gfx::common::DrawElements
 */
enum class index_type : GLenum
{
    UBYTE = GL_UNSIGNED_BYTE,   /**< 8-bit indices (up to 255 vertices). */
    USHORT = GL_UNSIGNED_SHORT, /**< 16-bit indices (up to 65 535 vertices). */
    UINT = GL_UNSIGNED_INT      /**< 32-bit indices (meshes beyond 65k vertices). */
};
} // namespace enums
} // namespace sog::gfx
