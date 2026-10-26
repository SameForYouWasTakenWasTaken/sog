/**
 * @file CommonDrawTests.cpp
 * @brief Pixel-verifies the sog::gfx::common draw entry points (DrawArrays, DrawElements and their
 * instanced variants) plus ViewportToActiveTarget, by rasterizing into an offscreen framebuffer
 * and reading the result back with glReadPixels.
 *
 * @note Pixel sampling matters for the DrawElements cases: each full-screen quad is two triangles
 * whose shared diagonal passes through the exact center of the target, so a test that draws only
 * one of the two triangles must sample a pixel clearly inside it (e.g. (24, 8) in the lower-right
 * half of a 32x32 target) rather than the center.
 */

#include <TestSupport.hpp>

#include <array>
#include <cstddef>
#include <span>

using sog::FragmentShader;
using sog::Index;
using sog::IndexBuffer;
using sog::ShaderProgram;
using sog::VertexArray;
using sog::VertexBuffer;
using sog::VertexShader;
using sog::math::Vec4;

namespace
{
/// Target size shared by the draw tests; the quad fills all of it, so any pixel proves a raster.
constexpr int SCENE_SIZE = 32;

/**
 * @brief A ready-to-draw scene: a color-interpolating pipeline plus a 32x32 offscreen target.
 *
 * upload() fills the vertex buffer and wires the vertex array; clearAndBind() resets the target
 * to black and binds it (with a matching viewport and the scene's program) so each test starts
 * from a known pixel state.
 */
struct Scene
{
    VertexBuffer vertexBuffer; ///< Geometry uploaded by the test.
    VertexArray vertexArray;   ///< Layout + (optional) index buffer for the draw.
    ShaderProgram program;     ///< COLOR_VERT + VERTEX_COLOR_FRAG, linked once per scene.
    sogtest::OffscreenTarget target; ///< Offscreen color/depth target for readback.

    /// Links the color pipeline (fields initialize before the body runs).
    Scene() : program{VertexShader{sogtest::COLOR_VERT}, FragmentShader{sogtest::VERTEX_COLOR_FRAG}}
    {
    }

    /// Uploads vertices, configures the VAO and allocates the offscreen target.
    void upload(const std::span<const sog::Vertex>& vertices)
    {
        vertexBuffer.set(vertices);
        vertexArray.create();
        vertexArray.setVertexBuffer(vertexBuffer, 0, sog::Vertex::stride());
        vertexArray.setAttributes(sog::Vertex::attributes());
        target = sogtest::OffscreenTarget::create(SCENE_SIZE, SCENE_SIZE);
    }

    /// Clears the bound target to opaque black, makes the scene's program current and binds the
    /// vertex array so the test can issue draw calls directly.
    void clearAndBind()
    {
        target.bind();
        sog::Window::clear(sog::ClearBuffer::Color, sog::Color{0.f, 0.f, 0.f, 255.f});
        program.use();
        vertexArray.bind();
    }
};

/// Reads the scene's center pixel (any pixel would do for a full-quad draw: it covers everything).
sog::Color centerPixel(const Scene& scene) { return scene.target.readPixel(SCENE_SIZE / 2, SCENE_SIZE / 2); }

/// Vertices for one full-screen quad (two triangles) with every vertex in the same flat color.
std::vector<sog::Vertex> coloredQuad(const Vec4& color)
{
    return sogtest::fullScreenQuad(color);
}
} // namespace

// ======== DrawArrays ======== //

TEST(CommonDraw, DrawArraysRasterizesTheRequestedVertexRange)
{
    Scene scene;

    // Vertices 0-5: red quad; vertices 6-11: blue quad (same positions, different color).
    auto vertices = coloredQuad({1.f, 0.f, 0.f, 1.f});
    const auto blueQuad = coloredQuad({0.f, 0.f, 1.f, 1.f});
    vertices.insert(vertices.end(), blueQuad.begin(), blueQuad.end());
    scene.upload(std::span<const sog::Vertex>{vertices});

    scene.clearAndBind();
    sog::gfx::common::DrawArrays(sog::gfx::draw_mode::TRIANGLES, 0, 6);
    sogtest::expectPixelNear(centerPixel(scene), 255, 0, 0, 255);

    sog::Window::clear(sog::ClearBuffer::Color, sog::Color{0.f, 0.f, 0.f, 255.f});
    sog::gfx::common::DrawArrays(sog::gfx::draw_mode::TRIANGLES, 6, 6);
    sogtest::expectPixelNear(centerPixel(scene), 0, 0, 255, 255);

    sog::FrameBuffer::unbind();
    expectNoGLError();
}

TEST(CommonDraw, DrawArraysWithZeroCountDrawsNothing)
{
    Scene scene;
    scene.upload(std::span<const sog::Vertex>{coloredQuad({1.f, 0.f, 0.f, 1.f})});

    scene.clearAndBind();
    sog::gfx::common::DrawArrays(sog::gfx::draw_mode::TRIANGLES, 0, 0);

    sogtest::expectPixelNear(centerPixel(scene), 0, 0, 0, 255, "no vertices must mean no raster");
    sog::FrameBuffer::unbind();
    expectNoGLError();
}

TEST(CommonDraw, DrawArraysHonoursTheFirstVertexOffset)
{
    Scene scene;

    auto vertices = coloredQuad({0.f, 1.f, 0.f, 1.f}); // green quad first
    const auto redQuad = coloredQuad({1.f, 0.f, 0.f, 1.f});
    vertices.insert(vertices.end(), redQuad.begin(), redQuad.end());
    scene.upload(std::span<const sog::Vertex>{vertices});

    scene.clearAndBind();
    sog::gfx::common::DrawArrays(sog::gfx::draw_mode::TRIANGLES, 6, 6); // skip the green quad

    sogtest::expectPixelNear(centerPixel(scene), 255, 0, 0, 255);
    sog::FrameBuffer::unbind();
    expectNoGLError();
}

// ======== DrawElements ======== //

TEST(CommonDraw, DrawElementsRasterizesThroughTheIndexBuffer)
{
    Scene scene;
    auto vertices = coloredQuad({1.f, 0.f, 0.f, 1.f});
    const auto blueQuad = coloredQuad({0.f, 0.f, 1.f, 1.f});
    vertices.insert(vertices.end(), blueQuad.begin(), blueQuad.end());
    scene.upload(std::span<const sog::Vertex>{vertices});

    // Index buffer selects only the blue quad's lower-right triangle.
    IndexBuffer indices;
    indices.set(std::span<const Index>{std::array<Index, 3>{6, 7, 8}});
    scene.vertexArray.setIndexBuffer(indices);
    ASSERT_TRUE(scene.vertexArray.indexed());

    scene.clearAndBind();
    sog::gfx::common::DrawElements(sog::gfx::draw_mode::TRIANGLES, 3, sog::gfx::index_type::UINT);

    // Sample inside the drawn half-triangle, away from the shared quad diagonal.
    sogtest::expectPixelNear(scene.target.readPixel(24, 8), 0, 0, 255, 255);
    sog::FrameBuffer::unbind();
    expectNoGLError();
}

TEST(CommonDraw, DrawElementsDrawsOnlyTheRequestedIndexCount)
{
    Scene scene;
    auto vertices = coloredQuad({1.f, 0.f, 0.f, 1.f});
    const auto blueQuad = coloredQuad({0.f, 0.f, 1.f, 1.f});
    vertices.insert(vertices.end(), blueQuad.begin(), blueQuad.end());
    scene.upload(std::span<const sog::Vertex>{vertices});

    // Six indices: first triangle red, second (same footprint, other quad) blue.
    IndexBuffer indices;
    indices.set(std::span<const Index>{std::array<Index, 6>{0, 1, 2, 6, 7, 8}});
    scene.vertexArray.setIndexBuffer(indices);

    scene.clearAndBind();
    sog::gfx::common::DrawElements(sog::gfx::draw_mode::TRIANGLES, 3, sog::gfx::index_type::UINT);
    sogtest::expectPixelNear(scene.target.readPixel(24, 8), 255, 0, 0, 255,
                             "only the first three indices must be consumed");

    sog::Window::clear(sog::ClearBuffer::Color, sog::Color{0.f, 0.f, 0.f, 255.f});
    sog::gfx::common::DrawElements(sog::gfx::draw_mode::TRIANGLES, 6, sog::gfx::index_type::UINT);
    sogtest::expectPixelNear(scene.target.readPixel(24, 8), 0, 0, 255, 255,
                             "all six indices: the blue triangle draws over the red one");

    sog::FrameBuffer::unbind();
    expectNoGLError();
}

TEST(CommonDraw, DrawElementsWithZeroCountDrawsNothing)
{
    Scene scene;
    scene.upload(std::span<const sog::Vertex>{coloredQuad({1.f, 0.f, 0.f, 1.f})});

    IndexBuffer indices;
    indices.set(std::span<const Index>{std::array<Index, 3>{0, 1, 2}});
    scene.vertexArray.setIndexBuffer(indices);

    scene.clearAndBind();
    sog::gfx::common::DrawElements(sog::gfx::draw_mode::TRIANGLES, 0, sog::gfx::index_type::UINT);

    sogtest::expectPixelNear(centerPixel(scene), 0, 0, 0, 255);
    sog::FrameBuffer::unbind();
    expectNoGLError();
}

// ======== Instanced draws ======== //

TEST(CommonDraw, DrawArraysInstancedDrawsOncePerInstance)
{
    Scene scene;
    scene.upload(std::span<const sog::Vertex>{coloredQuad({1.f, 0.f, 0.f, 1.f})});

    scene.clearAndBind();

    // No instances: nothing may reach the framebuffer.
    sog::gfx::common::DrawArraysInstanced(sog::gfx::draw_mode::TRIANGLES, 0, 6, 0);
    sogtest::expectPixelNear(centerPixel(scene), 0, 0, 0, 255, "zero instances must not rasterize");

    // One or more instances: the quad is drawn (identical geometry per instance, since the engine
    // attribute layout carries no divisors).
    sog::Window::clear(sog::ClearBuffer::Color, sog::Color{0.f, 0.f, 0.f, 255.f});
    sog::gfx::common::DrawArraysInstanced(sog::gfx::draw_mode::TRIANGLES, 0, 6, 3);
    sogtest::expectPixelNear(centerPixel(scene), 255, 0, 0, 255);

    sog::FrameBuffer::unbind();
    expectNoGLError();
}

TEST(CommonDraw, DrawElementsInstancedDrawsOncePerInstance)
{
    Scene scene;
    auto vertices = coloredQuad({1.f, 0.f, 0.f, 1.f});
    const auto blueQuad = coloredQuad({0.f, 0.f, 1.f, 1.f});
    vertices.insert(vertices.end(), blueQuad.begin(), blueQuad.end());
    scene.upload(std::span<const sog::Vertex>{vertices});

    IndexBuffer indices;
    indices.set(std::span<const Index>{std::array<Index, 3>{6, 7, 8}});
    scene.vertexArray.setIndexBuffer(indices);

    scene.clearAndBind();

    sog::gfx::common::DrawElementsInstanced(sog::gfx::draw_mode::TRIANGLES, 3,
                                             sog::gfx::index_type::UINT, nullptr, 0);
    sogtest::expectPixelNear(centerPixel(scene), 0, 0, 0, 255, "zero instances must not rasterize");

    sog::Window::clear(sog::ClearBuffer::Color, sog::Color{0.f, 0.f, 0.f, 255.f});
    sog::gfx::common::DrawElementsInstanced(sog::gfx::draw_mode::TRIANGLES, 3,
                                             sog::gfx::index_type::UINT, nullptr, 2);
    // The indexed geometry is the lower-right triangle; sample inside it.
    sogtest::expectPixelNear(scene.target.readPixel(24, 8), 0, 0, 255, 255);

    sog::FrameBuffer::unbind();
    expectNoGLError();
}

// ======== Per-vertex color interpolation ======== //

TEST(CommonDraw, InterpolatedVertexColorsReachTheFragmentStage)
{
    Scene scene;

    const std::array<sog::Vertex, 6> gradient{
        sog::Vertex{{-1.f, -1.f, 0.f}, {1.f, 0.f, 0.f, 1.f}, {0.f, 0.f}}, // bottom: red
        sog::Vertex{{1.f, -1.f, 0.f}, {1.f, 0.f, 0.f, 1.f}, {1.f, 0.f}},  // bottom: red
        sog::Vertex{{1.f, 1.f, 0.f}, {0.f, 1.f, 0.f, 1.f}, {1.f, 1.f}},   // top: green
        sog::Vertex{{-1.f, -1.f, 0.f}, {1.f, 0.f, 0.f, 1.f}, {0.f, 0.f}}, // bottom: red
        sog::Vertex{{1.f, 1.f, 0.f}, {0.f, 1.f, 0.f, 1.f}, {1.f, 1.f}},   // top: green
        sog::Vertex{{-1.f, 1.f, 0.f}, {0.f, 1.f, 0.f, 1.f}, {0.f, 1.f}},  // top: green
    };
    scene.upload(std::span<const sog::Vertex>{gradient});

    scene.clearAndBind();
    sog::gfx::common::DrawArrays(sog::gfx::draw_mode::TRIANGLES, 0, 6);

    // Bottom rows are red-dominant, top rows green-dominant; the middle blends smoothly.
    const sog::Color bottom = scene.target.readPixel(SCENE_SIZE / 2, 2);
    EXPECT_GT(bottom.r, 0.8f) << "the bottom of the quad must still be red";
    EXPECT_LT(bottom.g, 0.2f);

    const sog::Color top = scene.target.readPixel(SCENE_SIZE / 2, SCENE_SIZE - 2);
    EXPECT_GT(top.g, 0.8f) << "the top of the quad must be green";
    EXPECT_LT(top.r, 0.2f);

    const sog::Color middle = scene.target.readPixel(SCENE_SIZE / 2, SCENE_SIZE / 2);
    EXPECT_GT(middle.r, 0.3f) << "the middle must blend both vertex colors";
    EXPECT_GT(middle.g, 0.3f);

    sog::FrameBuffer::unbind();
    expectNoGLError();
}

// ======== Viewport helper ======== //

TEST(CommonDraw, ViewportToActiveTargetSetsTheGLViewport)
{
    sog::gfx::common::ViewportToActiveTarget(3, 4, 320, 240);

    GLint viewport[4] = {0, 0, 0, 0};
    glGetIntegerv(GL_VIEWPORT, viewport);
    EXPECT_EQ(viewport[0], 3);
    EXPECT_EQ(viewport[1], 4);
    EXPECT_EQ(viewport[2], 320);
    EXPECT_EQ(viewport[3], 240);
    expectNoGLError();
}
