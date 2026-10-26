/**
 * @file Texture2DTests.cpp
 * @brief Exercises gfx::Texture2D: GL texture creation, raw storage allocation with sampler
 * settings, JPEG loading through stb_image, binding, release and move semantics.
 */

#include <TestSupport.hpp>

#include <array>
#include <cstdint>
#include <filesystem>
#include <type_traits>
#include <utility>
#include <vector>

using sog::Texture2D;
using sog::gfx::TextureSettings;
using sog::gfx::pixel_format;
using sog::gfx::pixel_type;
using sog::gfx::texture_internal_format;
using sog::gfx::texture_mag_filters;
using sog::gfx::texture_min_filters;
using sog::gfx::texture_params;
using sog::graphics_error;
using sog::graphics_error_code;

namespace
{
/// 2x2 RGBA8 image: bottom row red/green, top row blue/white (GL row 0 is the bottom row).
constexpr std::array<std::uint8_t, 16> QUAD_PIXELS{
    255, 0,   0,   255, // (0, 0) red
    0,   255, 0,   255, // (1, 0) green
    0,   0,   255, 255, // (0, 1) blue
    255, 255, 255, 255, // (1, 1) white
};

/// Uploads QUAD_PIXELS as a 2x2 RGBA8 texture with default settings.
Texture2D makeQuadTexture()
{
    return Texture2D{QUAD_PIXELS.data(), 2, 2, texture_internal_format::RGBA8, pixel_format::RGBA,
                     pixel_type::UnsignedByte, TextureSettings{}};
}

/// Queries a level-0 texture parameter with DSA (no binding needed).
GLint levelParameter(const Texture2D& texture, GLenum parameter)
{
    GLint value = 0;
    glGetTextureLevelParameteriv(texture.getHandle().get(), 0, parameter, &value);
    return value;
}

/// Queries a sampler parameter of the texture object with DSA.
GLint textureParameter(const Texture2D& texture, GLenum parameter)
{
    GLint value = 0;
    glGetTextureParameteriv(texture.getHandle().get(), parameter, &value);
    return value;
}
} // namespace

// ======== Type properties ======== //

TEST(Texture2D, IsMoveOnly)
{
    static_assert(!std::is_copy_constructible_v<Texture2D>, "Texture2D must not be copyable");
    static_assert(!std::is_copy_assignable_v<Texture2D>, "Texture2D must not be copy assignable");
    static_assert(std::is_nothrow_move_constructible_v<Texture2D>,
                  "Texture2D must be nothrow move constructible");
    static_assert(std::is_nothrow_move_assignable_v<Texture2D>,
                  "Texture2D must be nothrow move assignable");
    SUCCEED();
}

// ======== Construction ======== //

TEST(Texture2D, DefaultConstructedCreatesATextureObject)
{
    const Texture2D texture;

    EXPECT_TRUE(texture.getHandle().valid());
    EXPECT_NE(texture.getHandle().get(), 0u);
}

TEST(Texture2D, EmptyAllocatesStorageOfTheRequestedSize)
{
    const Texture2D texture = Texture2D::empty(4, 4);

    EXPECT_EQ(levelParameter(texture, GL_TEXTURE_WIDTH), 4);
    EXPECT_EQ(levelParameter(texture, GL_TEXTURE_HEIGHT), 4);
    expectNoGLError();
}

TEST(Texture2D, EmptyRejectsNonPositiveDimensions)
{
    EXPECT_THROW(Texture2D::empty(0, 4), graphics_error);
    EXPECT_THROW(Texture2D::empty(4, -1), graphics_error);
    drainGLErrors();
}

// ======== createFromRaw() ======== //

TEST(Texture2D, CreateFromRawAllocatesStorageAndUploadsPixels)
{
    const Texture2D texture = makeQuadTexture();

    EXPECT_EQ(levelParameter(texture, GL_TEXTURE_WIDTH), 2);
    EXPECT_EQ(levelParameter(texture, GL_TEXTURE_HEIGHT), 2);
    EXPECT_EQ(levelParameter(texture, GL_TEXTURE_INTERNAL_FORMAT), GL_RGBA8);
    expectNoGLError();
}

TEST(Texture2D, CreateFromRawAppliesSamplerSettings)
{
    TextureSettings settings;
    settings.wrapS = texture_params::CLAMP_TO_EDGE;
    settings.wrapT = texture_params::MIRRORED_REPEAT;
    settings.minFilter = texture_min_filters::NEAREST;
    settings.magFilter = texture_mag_filters::NEAREST;

    const Texture2D texture{QUAD_PIXELS.data(), 2, 2, texture_internal_format::RGBA8,
                            pixel_format::RGBA, pixel_type::UnsignedByte, settings};

    EXPECT_EQ(textureParameter(texture, GL_TEXTURE_WRAP_S), GL_CLAMP_TO_EDGE);
    EXPECT_EQ(textureParameter(texture, GL_TEXTURE_WRAP_T), GL_MIRRORED_REPEAT);
    EXPECT_EQ(textureParameter(texture, GL_TEXTURE_MIN_FILTER), GL_NEAREST);
    EXPECT_EQ(textureParameter(texture, GL_TEXTURE_MAG_FILTER), GL_NEAREST);
    expectNoGLError();
}

TEST(Texture2D, CreateFromRawTwiceOnOneTextureIsRejected)
{
    // Documented quirk: storage is immutable (glTextureStorage2D), so a second createFromRaw on
    // the same object surfaces GL_INVALID_OPERATION instead of reallocating.
    Texture2D texture = makeQuadTexture();
    drainGLErrors();

    texture.createFromRaw(QUAD_PIXELS.data(), 2, 2, texture_internal_format::RGBA8,
                          pixel_format::RGBA, pixel_type::UnsignedByte, TextureSettings{});

    EXPECT_NE(glGetError(), static_cast<GLenum>(GL_NO_ERROR))
        << "re-allocating immutable texture storage must fail";
    drainGLErrors();
}

// ======== createFromFile() ======== //

TEST(Texture2D, CreateFromFileLoadsTheJpegAsset)
{
    const std::filesystem::path path{std::string{SOG_TEST_ASSET_DIR} + "/Textures/test.jpg"};

    const Texture2D texture{path, TextureSettings{}};

    EXPECT_TRUE(texture.getHandle().valid());
    EXPECT_GT(levelParameter(texture, GL_TEXTURE_WIDTH), 0);
    EXPECT_GT(levelParameter(texture, GL_TEXTURE_HEIGHT), 0);
    expectNoGLError();
}

TEST(Texture2D, CreateFromFileThrowsTextureLoadFailedWhenMissing)
{
    Texture2D texture;
    drainGLErrors();

    try
    {
        texture.createFromFile(std::filesystem::path{"no/such/image.jpg"}, TextureSettings{});
        FAIL() << "loading a missing image must throw";
    }
    catch (const graphics_error& error)
    {
        EXPECT_EQ(error.code(), graphics_error_code::texture_load_failed);
    }
    drainGLErrors();
}

// ======== bind() / release() ======== //

TEST(Texture2D, BindAttachesToSamplerUnitZero)
{
    const Texture2D texture = makeQuadTexture();

    texture.bind(0);

    GLint bound = 0;
    glGetIntegeri_v(GL_TEXTURE_BINDING_2D, 0, &bound);
    EXPECT_EQ(static_cast<GLuint>(bound), texture.getHandle().get());
    expectNoGLError();
}

TEST(Texture2D, BindAfterReleaseThrowsTextureNotBound)
{
    Texture2D texture = makeQuadTexture();
    texture.release();

    EXPECT_FALSE(texture.getHandle().valid());

    try
    {
        texture.bind(0);
        FAIL() << "binding a released texture must throw";
    }
    catch (const graphics_error& error)
    {
        EXPECT_EQ(error.code(), graphics_error_code::texture_not_bound);
    }
}

// ======== Move semantics ======== //

TEST(Texture2D, MoveConstructorTransfersTheTexture)
{
    Texture2D source = makeQuadTexture();
    const GLuint sourceId = source.getHandle().get();

    Texture2D target{std::move(source)};

    EXPECT_EQ(target.getHandle().get(), sourceId);
    EXPECT_FALSE(source.getHandle().valid()) << "moving must zero the source handle";

    EXPECT_THROW(source.bind(0), graphics_error);
}

TEST(Texture2D, MoveAssignmentReleasesTheTargetsOldTexture)
{
    Texture2D source = makeQuadTexture();
    const GLuint sourceId = source.getHandle().get();

    Texture2D target = Texture2D::empty(8, 8);
    target = std::move(source);

    EXPECT_EQ(target.getHandle().get(), sourceId);
    EXPECT_EQ(levelParameter(target, GL_TEXTURE_WIDTH), 2) << "target now owns the source texture";
    EXPECT_FALSE(source.getHandle().valid());
    expectNoGLError();
}
