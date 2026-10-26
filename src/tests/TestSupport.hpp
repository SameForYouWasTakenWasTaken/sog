/**
 * @file TestSupport.hpp
 * @brief Shared GoogleTest fixtures and helpers for the SOG suite: a hidden OpenGL context, an
 * offscreen render target for pixel readback, minimal GLSL sources and GL-error/vector assertions.
 *
 * All GL-touching tests derive from GLTest, which creates (once, shared across the whole process)
 * a hidden GLFW window with a real 4.6 core context. Tests that need their own window (e.g.
 * WindowTests) derive from GLTest, create a local Window, and rely on TearDown() to restore the
 * shared context so the next test starts from a known-current context.
 */

#pragma once
#include <gtest/gtest.h>
#include <sog/sog.hpp>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace sogtest
{
/**
 * @brief Returns the process-wide hidden GL context window, creating it on first use.
 * @return Reference to the shared sog::Window (4.6 core, GLFW_VISIBLE false).
 *
 * The window is created with a non-default context that requests a debug context (the engine
 * already sets GLFW_OPENGL_DEBUG_CONTEXT) so debug-output tests behave identically to a real
 * application.
 */
[[nodiscard]] sog::Window& sharedContext();

/// Makes the shared hidden context current again (call after a test created its own Window).
void makeSharedContextCurrent();

/// Destroys the shared context; called once from main() after RUN_ALL_TESTS() returns.
void destroySharedContext();

/**
 * @brief Clears any pending GL errors and asserts the error queue is empty.
 *
 * Tests call this at the end of any sequence of GL calls to prove the engine issued no erroneous
 * GL command. Pending errors from a test's own deliberate triggering must be drained beforehand.
 */
void expectNoGLError();

/// Drains and discards every pending GL error (used by tests that provoke one on purpose).
void drainGLErrors();

/// Asserts two float vectors are component-wise within a tolerance (for basis vectors).
void expectVecNear(const sog::math::Vec3& actual, const sog::math::Vec3& expected, float tolerance);
/// Asserts two float vectors are component-wise within a tolerance (for colors / vec4).
void expectVecNear(const sog::math::Vec4& actual, const sog::math::Vec4& expected, float tolerance);
/// Asserts two 4x4 matrices are element-wise within a tolerance.
void expectMatNear(const sog::math::Mat4& actual, const sog::math::Mat4& expected, float tolerance);

/**
 * @brief Builds two triangles covering the whole clip-space square [-1, 1]^2.
 * @param color Flat color given to all six vertices (position + color interleaving follows
 * sog::Vertex, so the result plugs straight into a VertexBuffer + Vertex::attributes()).
 * @return Six sog::Vertex values forming two triangles.
 */
[[nodiscard]] std::vector<sog::Vertex> fullScreenQuad(const sog::math::Vec4& color);

/**
 * @brief Asserts a read-back pixel equals an expected 8-bit color within one byte per channel.
 * @param actual Pixel returned by OffscreenTarget::readPixel.
 * @param r Expected red, 0-255.
 * @param g Expected green, 0-255.
 * @param b Expected blue, 0-255.
 * @param a Expected alpha, 0-255.
 * @param context Optional failure narrative appended to each channel comparison (the helper
 * returns void, so callers pass context here instead of streaming into the result).
 */
void expectPixelNear(const sog::Color& actual, std::uint8_t r, std::uint8_t g, std::uint8_t b,
                     std::uint8_t a, const char* context = nullptr);

/// Minimal vertex shader: passes a vec3 position straight through (no uniforms, no transforms).
inline constexpr std::string_view PASS_THROUGH_VERT = R"glsl(
#version 460 core
layout (location = 0) in vec3 aPos;
void main() { gl_Position = vec4(aPos, 1.0); }
)glsl";

/// Minimal fragment shader: outputs a solid red color.
inline constexpr std::string_view SOLID_RED_FRAG = R"glsl(
#version 460 core
out vec4 FragColor;
void main() { FragColor = vec4(1.0, 0.0, 0.0, 1.0); }
)glsl";

/**
 * @brief Vertex shader feeding both position and per-vertex color to the fragment stage.
 *
 * Locations match sog::Vertex::attributes(): position at 0, color at 1. The interpolated color is
 * passed to the fragment stage as vColor for VERTEX_COLOR_FRAG to output.
 */
inline constexpr std::string_view COLOR_VERT = R"glsl(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
out vec4 vColor;
void main()
{
    gl_Position = vec4(aPos, 1.0);
    vColor = aColor;
}
)glsl";

/**
 * @brief Fragment shader echoing the interpolated per-vertex color, so tests can prove the whole
 * vertex -> raster -> fragment pipeline ran by reading a pixel back.
 */
inline constexpr std::string_view VERTEX_COLOR_FRAG = R"glsl(
#version 460 core
in vec4 vColor;
out vec4 FragColor;
void main() { FragColor = vColor; }
)glsl";

/**
 * @brief Fragment shader whose output is the product of seven uniforms, each kept active.
 *
 * Every uniform contributes to FragColor, so the linker can not eliminate any of them and every
 * uniform::set* helper can be verified against a read-back pixel. Tests supply neutral values
 * (identity matrices, unit tint/boost/int, zero offset) for the uniforms they are not exercising.
 */
inline constexpr std::string_view UNIFORM_FRAG = R"glsl(
#version 460 core
uniform vec4 uColor;
uniform vec2 uOffset;
uniform vec3 uTint;
uniform mat3 uMat3;
uniform mat4 uMat4;
uniform float uBoost;
uniform int uInt;
out vec4 FragColor;
void main()
{
    vec4 composed = uColor;
    composed.xy += uOffset;
    composed.rgb *= uTint;
    composed = vec4(uMat3 * composed.xyz, composed.w);
    composed = uMat4 * composed;
    composed *= uBoost;
    composed *= float(uInt);
    FragColor = composed;
}
)glsl";

/// Fragment shader sampling a 2D texture (unit 0); used to verify texture binding and loading.
inline constexpr std::string_view TEXTURE_FRAG = R"glsl(
#version 460 core
uniform sampler2D uTexture;
out vec4 FragColor;
void main() { FragColor = texture(uTexture, vec2(0.5)); }
)glsl";

/**
 * @brief Offscreen render target: an FBO with one RGBA8 color texture and a depth renderbuffer.
 *
 * Rendering into target() then reading a pixel back proves draw calls actually rasterized
 * something, without depending on anything visible on screen. Created through create().
 */
struct OffscreenTarget
{
    sog::FrameBuffer frameBuffer;        ///< The FBO owning the attachments.
    sog::Texture2D color;                ///< RGBA8 color attachment (sampled for readback).
    sog::RenderBuffer depth{GL_DEPTH24_STENCIL8, 0, 0}; ///< Depth/stencil attachment.
    int width = 0;                       ///< Width of the color attachment (and viewport).
    int height = 0;                      ///< Height of the color attachment (and viewport).

    /**
     * @brief Builds a complete (isComplete() true) offscreen target of the given size.
     * @param width Color texture / renderbuffer width in pixels.
     * @param height Color texture / renderbuffer height in pixels.
     * @return A ready-to-render target; bind it via frameBuffer.bind().
     */
    static OffscreenTarget create(int width, int height);

    /// Binds the FBO and sets the viewport to its full size so draws land inside it.
    void bind() const;

    /**
     * @brief Reads one pixel out of the color attachment as 8-bit channels.
     * @param x Column, in pixels, with (0, 0) at the framebuffer's bottom-left corner.
     * @param y Row, in pixels, with (0, 0) at the framebuffer's bottom-left corner.
     * @return The pixel's RGBA channels as normalized floats (0.0 - 1.0).
     */
    [[nodiscard]] sog::Color readPixel(int x, int y) const;

    /// Reads a whole scanline row (one pixel per column) as 8-bit RGBA, left to right.
    [[nodiscard]] std::vector<sog::Color> readRow(int y) const;
};

/**
 * @brief Fixture for tests that need a current OpenGL context.
 *
 * Ensures the shared hidden context is current before each test and clears pending GL errors
 * afterwards so one test's stray error cannot leak into the next. Tests may create additional
 * Windows on top of this; TearDown() always restores the shared context.
 */
class GLTest : public ::testing::Test
{
  protected:
    /// Makes the shared context current before the test body runs.
    void SetUp() override;

    /// Restores the shared context and asserts the test left no pending GL error.
    void TearDown() override;
};
} // namespace sogtest

// Test bodies call the assertion helpers without qualification (they live in sogtest to keep the
// support API grouped; these declarations keep TEST() bodies terse).
using sogtest::drainGLErrors;
using sogtest::expectMatNear;
using sogtest::expectNoGLError;
using sogtest::expectPixelNear;
using sogtest::expectVecNear;
using sogtest::fullScreenQuad;
