/**
 * @file TestSupport.cpp
 * @brief Implements the shared hidden GL context, the offscreen render target and the assertion
 * helpers declared in TestSupport.hpp.
 */

#include "TestSupport.hpp"

#include <glad/gl.h>

#include <cmath>
#include <memory>

namespace sogtest
{
namespace
{
/// The single hidden context window shared by every GL-touching test in the process.
std::unique_ptr<sog::Window> g_SharedContext;
} // namespace

sog::Window& sharedContext()
{
    if (!g_SharedContext)
        g_SharedContext = std::make_unique<sog::Window>();
    return *g_SharedContext;
}

void makeSharedContextCurrent()
{
    sog::Window& window = sharedContext();
    if (auto* handle = static_cast<GLFWwindow*>(window.getNativeHandle()))
        glfwMakeContextCurrent(handle);
}

void destroySharedContext() { g_SharedContext.reset(); }

void expectNoGLError()
{
    const GLenum error = glGetError();
    EXPECT_EQ(error, static_cast<GLenum>(GL_NO_ERROR))
        << "OpenGL reported error 0x" << std::hex << error << std::dec;
}

void drainGLErrors()
{
    while (glGetError() != GL_NO_ERROR)
    {
        // Keep draining until the queue is empty.
    }
}

void expectVecNear(const sog::math::Vec3& actual, const sog::math::Vec3& expected, float tolerance)
{
    EXPECT_NEAR(actual.x, expected.x, tolerance);
    EXPECT_NEAR(actual.y, expected.y, tolerance);
    EXPECT_NEAR(actual.z, expected.z, tolerance);
}

void expectVecNear(const sog::math::Vec4& actual, const sog::math::Vec4& expected, float tolerance)
{
    EXPECT_NEAR(actual.x, expected.x, tolerance);
    EXPECT_NEAR(actual.y, expected.y, tolerance);
    EXPECT_NEAR(actual.z, expected.z, tolerance);
    EXPECT_NEAR(actual.w, expected.w, tolerance);
}

void expectMatNear(const sog::math::Mat4& actual, const sog::math::Mat4& expected, float tolerance)
{
    for (int column = 0; column < 4; ++column)
        for (int row = 0; row < 4; ++row)
            EXPECT_NEAR(actual[column][row], expected[column][row], tolerance)
                << "mismatch at column " << column << ", row " << row;
}

std::vector<sog::Vertex> fullScreenQuad(const sog::math::Vec4& color)
{
    const sog::math::Vec3 bottomLeft{-1.f, -1.f, 0.f};
    const sog::math::Vec3 bottomRight{1.f, -1.f, 0.f};
    const sog::math::Vec3 topLeft{-1.f, 1.f, 0.f};
    const sog::math::Vec3 topRight{1.f, 1.f, 0.f};

    return {
        sog::Vertex{bottomLeft, color, {0.f, 0.f}},  sog::Vertex{bottomRight, color, {1.f, 0.f}},
        sog::Vertex{topRight, color, {1.f, 1.f}},    sog::Vertex{bottomLeft, color, {0.f, 0.f}},
        sog::Vertex{topRight, color, {1.f, 1.f}},    sog::Vertex{topLeft, color, {0.f, 1.f}},
    };
}

void expectPixelNear(const sog::Color& actual, std::uint8_t r, std::uint8_t g, std::uint8_t b,
                     std::uint8_t a, const char* context)
{
    // One byte of slack absorbs 8-bit rasterization rounding.
    constexpr float kTolerance = 1.5f / 255.f;
    const char* why = context != nullptr ? context : "";
    EXPECT_NEAR(actual.r, static_cast<float>(r) / 255.f, kTolerance) << "red channel. " << why;
    EXPECT_NEAR(actual.g, static_cast<float>(g) / 255.f, kTolerance) << "green channel. " << why;
    EXPECT_NEAR(actual.b, static_cast<float>(b) / 255.f, kTolerance) << "blue channel. " << why;
    EXPECT_NEAR(actual.a, static_cast<float>(a) / 255.f, kTolerance) << "alpha channel. " << why;
}

OffscreenTarget OffscreenTarget::create(int width, int height)
{
    OffscreenTarget target;
    target.width = width;
    target.height = height;
    target.color = sog::Texture2D::empty(width, height);
    target.depth = sog::RenderBuffer{GL_DEPTH24_STENCIL8, width, height};

    target.frameBuffer.attachTexture(sog::gfx::FramebufferAttachment::Color0, target.color.getHandle());
    target.frameBuffer.attachRenderbuffer(sog::gfx::FramebufferAttachment::DepthStencil,
                                          target.depth);

    EXPECT_TRUE(target.frameBuffer.isComplete())
        << "Offscreen FBO with a color texture and depth renderbuffer must be complete";

    return target;
}

void OffscreenTarget::bind() const
{
    frameBuffer.bind();
    sog::gfx::common::ViewportToActiveTarget(0, 0, static_cast<GLsizei>(width),
                                             static_cast<GLsizei>(height));
}

sog::Color OffscreenTarget::readPixel(int x, int y) const
{
    std::uint8_t rgba[4] = {0, 0, 0, 0};
    glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer.id());
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    return {static_cast<float>(rgba[0]) / 255.f, static_cast<float>(rgba[1]) / 255.f,
            static_cast<float>(rgba[2]) / 255.f, static_cast<float>(rgba[3]) / 255.f};
}

std::vector<sog::Color> OffscreenTarget::readRow(int y) const
{
    std::vector<std::uint8_t> rgba(static_cast<std::size_t>(width) * 4u, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer.id());
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, y, width, 1, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    std::vector<sog::Color> row;
    row.reserve(static_cast<std::size_t>(width));
    for (int x = 0; x < width; ++x)
    {
        const auto offset = static_cast<std::size_t>(x) * 4u;
        row.push_back({static_cast<float>(rgba[offset]) / 255.f,
                       static_cast<float>(rgba[offset + 1]) / 255.f,
                       static_cast<float>(rgba[offset + 2]) / 255.f,
                       static_cast<float>(rgba[offset + 3]) / 255.f});
    }
    return row;
}

void GLTest::SetUp()
{
    makeSharedContextCurrent();
    drainGLErrors();
}

void GLTest::TearDown()
{
    makeSharedContextCurrent();
    drainGLErrors();
}
} // namespace sogtest
