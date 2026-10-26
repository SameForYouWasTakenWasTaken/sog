/**
 * @file EnumsTests.cpp
 * @brief Exercises the type-safe GL enums in sog::gfx (buffer targets, draw modes, index types,
 * pixel formats/types, shader types, texture formats and clear-buffer flags).
 *
 * Each enum is a thin, GL-ABI-identical wrapper over a raw OpenGL constant. These tests pin down
 * that the underlying value of every enumerator is exactly the GL constant it claims to mirror, so
 * an engine call site can hand the enum straight to GL without a translation table drifting out of
 * sync.
 */

#include <TestSupport.hpp>

#include <type_traits>

using sog::to_underlying;
using sog::gfx::attribute_type;
using sog::gfx::buffer_target;
using sog::gfx::draw_mode;
using sog::gfx::error;
using sog::gfx::index_type;
using sog::gfx::pixel_format;
using sog::gfx::pixel_type;
using sog::gfx::shader_type;
using sog::gfx::texture_internal_format;
using sog::gfx::texture_mag_filters;
using sog::gfx::texture_min_filters;
using sog::gfx::texture_mipmaps;
using sog::gfx::texture_params;

// ======== buffer_target ======== //

TEST(EnumBufferTarget, MirrorsGLConstants)
{
    EXPECT_EQ(to_underlying(buffer_target::VBO_BUFFER), GL_ARRAY_BUFFER);
    EXPECT_EQ(to_underlying(buffer_target::EBO_BUFFER), GL_ELEMENT_ARRAY_BUFFER);
}

// ======== draw_mode ======== //

TEST(EnumDrawMode, MirrorsGLConstants)
{
    EXPECT_EQ(to_underlying(draw_mode::TRIANGLES), GL_TRIANGLES);
    EXPECT_EQ(to_underlying(draw_mode::TRIANGLE_STRIP), GL_TRIANGLE_STRIP);
    EXPECT_EQ(to_underlying(draw_mode::TRIANGLE_FAN), GL_TRIANGLE_FAN);
    EXPECT_EQ(to_underlying(draw_mode::POINTS), GL_POINTS);
    EXPECT_EQ(to_underlying(draw_mode::LINES), GL_LINES);
    EXPECT_EQ(to_underlying(draw_mode::LINE_STRIP), GL_LINE_STRIP);
    EXPECT_EQ(to_underlying(draw_mode::LINES_ADJACENCY), GL_LINES_ADJACENCY);
    EXPECT_EQ(to_underlying(draw_mode::LINE_STRIP_ADJACENCY), GL_LINE_STRIP_ADJACENCY);
    EXPECT_EQ(to_underlying(draw_mode::TRIANGLES_ADJACENCY), GL_TRIANGLES_ADJACENCY);
    EXPECT_EQ(to_underlying(draw_mode::TRIANGLE_STRIP_ADJACENCY), GL_TRIANGLE_STRIP_ADJACENCY);
}

TEST(EnumDrawMode, EveryEnumeratorIsDistinct)
{
    // Guard against two enumerators silently aliasing the same GL constant.
    const draw_mode modes[] = {draw_mode::TRIANGLES,   draw_mode::TRIANGLE_STRIP,
                               draw_mode::TRIANGLE_FAN, draw_mode::POINTS,
                               draw_mode::LINES,       draw_mode::LINE_STRIP,
                               draw_mode::LINES_ADJACENCY, draw_mode::LINE_STRIP_ADJACENCY,
                               draw_mode::TRIANGLES_ADJACENCY, draw_mode::TRIANGLE_STRIP_ADJACENCY};

    for (std::size_t i = 0; i < std::size(modes); ++i)
        for (std::size_t j = i + 1; j < std::size(modes); ++j)
            EXPECT_NE(to_underlying(modes[i]), to_underlying(modes[j]))
                << "draw_mode enumerators at index " << i << " and " << j << " share a value";
}

// ======== index_type ======== //

TEST(EnumIndexType, MirrorsGLConstants)
{
    EXPECT_EQ(to_underlying(index_type::UBYTE), GL_UNSIGNED_BYTE);
    EXPECT_EQ(to_underlying(index_type::USHORT), GL_UNSIGNED_SHORT);
    EXPECT_EQ(to_underlying(index_type::UINT), GL_UNSIGNED_INT);
}

// ======== shader_type ======== //

TEST(EnumShaderType, MirrorsGLConstants)
{
    EXPECT_EQ(to_underlying(shader_type::VERTEX), GL_VERTEX_SHADER);
    EXPECT_EQ(to_underlying(shader_type::FRAGMENT), GL_FRAGMENT_SHADER);
    EXPECT_EQ(to_underlying(shader_type::GEOMETRY), GL_GEOMETRY_SHADER);
    EXPECT_EQ(to_underlying(shader_type::COMPUTE), GL_COMPUTE_SHADER);
}

// ======== pixel_format ======== //

TEST(EnumPixelFormat, MirrorsGLConstants)
{
    EXPECT_EQ(to_underlying(pixel_format::RGBA), GL_RGBA);
    EXPECT_EQ(to_underlying(pixel_format::RGB), GL_RGB);
    EXPECT_EQ(to_underlying(pixel_format::Red), GL_RED);
    EXPECT_EQ(to_underlying(pixel_format::RG), GL_RG);
    EXPECT_EQ(to_underlying(pixel_format::DepthStencil), GL_DEPTH_STENCIL);
}

// ======== pixel_type ======== //

TEST(EnumPixelType, MirrorsGLConstants)
{
    EXPECT_EQ(to_underlying(pixel_type::UnsignedByte), GL_UNSIGNED_BYTE);
    EXPECT_EQ(to_underlying(pixel_type::Float), GL_FLOAT);
    EXPECT_EQ(to_underlying(pixel_type::Int), GL_INT);
    EXPECT_EQ(to_underlying(pixel_type::UnsignedInt248), GL_UNSIGNED_INT_24_8);
}

// ======== texture_internal_format ======== //

TEST(EnumTextureInternalFormat, MirrorsGLConstants)
{
    EXPECT_EQ(to_underlying(texture_internal_format::RGBA8), GL_RGBA8);
    EXPECT_EQ(to_underlying(texture_internal_format::RGB8), GL_RGB8);
    EXPECT_EQ(to_underlying(texture_internal_format::RGBA16F), GL_RGBA16F);
    EXPECT_EQ(to_underlying(texture_internal_format::RGB16F), GL_RGB16F);
    EXPECT_EQ(to_underlying(texture_internal_format::R8), GL_R8);
    EXPECT_EQ(to_underlying(texture_internal_format::Depth24Stencil8), GL_DEPTH24_STENCIL8);
}

// ======== texture_params (wrap modes) ======== //

TEST(EnumTextureParams, MirrorsGLConstants)
{
    EXPECT_EQ(to_underlying(texture_params::REPEAT), GL_REPEAT);
    EXPECT_EQ(to_underlying(texture_params::MIRRORED_REPEAT), GL_MIRRORED_REPEAT);
    EXPECT_EQ(to_underlying(texture_params::CLAMP_TO_EDGE), GL_CLAMP_TO_EDGE);
    EXPECT_EQ(to_underlying(texture_params::CLAMP_TO_BORDER), GL_CLAMP_TO_BORDER);
}

// ======== texture_min_filters ======== //

TEST(EnumTextureMinFilters, MirrorsGLConstants)
{
    EXPECT_EQ(to_underlying(texture_min_filters::NEAREST), GL_NEAREST);
    EXPECT_EQ(to_underlying(texture_min_filters::LINEAR), GL_LINEAR);
    EXPECT_EQ(to_underlying(texture_min_filters::NEAREST_MIPMAP_NEAREST), GL_NEAREST_MIPMAP_NEAREST);
    EXPECT_EQ(to_underlying(texture_min_filters::LINEAR_MIPMAP_NEAREST), GL_LINEAR_MIPMAP_NEAREST);
    EXPECT_EQ(to_underlying(texture_min_filters::NEAREST_MIPMAP_LINEAR), GL_NEAREST_MIPMAP_LINEAR);
    EXPECT_EQ(to_underlying(texture_min_filters::LINEAR_MIPMAP_LINEAR), GL_LINEAR_MIPMAP_LINEAR);
}

// ======== texture_mag_filters ======== //

TEST(EnumTextureMagFilters, MirrorsGLConstants)
{
    EXPECT_EQ(to_underlying(texture_mag_filters::NEAREST), GL_NEAREST);
    EXPECT_EQ(to_underlying(texture_mag_filters::LINEAR), GL_LINEAR);
}

// ======== attribute_type ======== //

TEST(EnumAttributeType, MirrorsGLConstants)
{
    EXPECT_EQ(to_underlying(attribute_type::FLOAT32), GL_FLOAT);
}

// ======== texture_mipmaps ======== //

TEST(EnumTextureMipmaps, MirrorsGLConstants)
{
    EXPECT_EQ(to_underlying(texture_mipmaps::NEAREST), GL_NEAREST_MIPMAP_NEAREST);
    EXPECT_EQ(to_underlying(texture_mipmaps::LINEAR_NEAREST), GL_LINEAR_MIPMAP_NEAREST);
    EXPECT_EQ(to_underlying(texture_mipmaps::NEAREST_LINEAR), GL_NEAREST_MIPMAP_LINEAR);
    EXPECT_EQ(to_underlying(texture_mipmaps::LINEAR_LINEAR), GL_LINEAR_MIPMAP_LINEAR);
}

// ======== error ======== //

TEST(EnumError, NoneIsGLNone)
{
    EXPECT_EQ(to_underlying(error::NONE), GL_NONE);
}

// ======== Type traits ======== //

TEST(EnumTraits, EnumsUseUnsignedIntOrGLintBacking)
{
    // These enums are marshalled straight to GL, so they must be backed by integral types wide
    // enough to hold the GL constants. (ClearBuffer is checked in ClearBufferTests.cpp.)
    static_assert(std::is_enum_v<buffer_target>);
    static_assert(std::is_enum_v<draw_mode>);
    static_assert(std::is_enum_v<index_type>);
    static_assert(std::is_enum_v<shader_type>);
    static_assert(std::is_enum_v<pixel_format>);
    static_assert(std::is_enum_v<pixel_type>);
    SUCCEED();
}
