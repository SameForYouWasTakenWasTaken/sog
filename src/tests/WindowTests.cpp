/**
 * @file WindowTests.cpp
 * @brief Exercises gfx::Window: construction with a hidden context, move-only ownership, the frame
 * lifecycle helpers, the engine event queue, the resize callback and Window::draw pixel output.
 *
 * Every case that creates a Window derives from GLTest so the shared context is restored in
 * TearDown() after the local window's context was made current (and then destroyed with it).
 */

#include <TestSupport.hpp>

#include <GLFW/glfw3.h>

#include <array>
#include <span>
#include <type_traits>
#include <utility>

using sogtest::GLTest;

using sog::ClearBuffer;
using sog::Color;
using sog::Event;
using sog::EventType;
using sog::FrameBuffer;
using sog::Index;
using sog::IndexBuffer;
using sog::gfx::RenderState;
using sog::ShaderProgram;
using sog::Texture2D;
using sog::VertexArray;
using sog::VertexBuffer;
using sog::Window;
using sog::graphics_error;
using sog::graphics_error_code;

// ======== Type properties ======== //

static_assert(!std::is_copy_constructible_v<Window>,
              "a window owns exactly one GLFWwindow: it must not be copyable");
static_assert(!std::is_copy_assignable_v<Window>,
              "a window owns exactly one GLFWwindow: it must not be copy assignable");
static_assert(std::is_nothrow_move_constructible_v<Window>,
              "Window must be nothrow move constructible");
static_assert(std::is_nothrow_move_assignable_v<Window>, "Window must be nothrow move assignable");

// ======== Construction ======== //

TEST_F(GLTest, DefaultConstructionYieldsAnOpenHiddenWindow)
{
    const Window window;

    EXPECT_NE(window.getNativeHandle(), nullptr);
    EXPECT_FALSE(window.shouldClose());
    EXPECT_GT(window.getWidth(), 0);
    EXPECT_GT(window.getHeight(), 0);
}

TEST_F(GLTest, CustomContextOverridesSize)
{
    sog::context::WindowContext context;
    context.width = 320;
    context.height = 240;

    const Window window{context};

    EXPECT_EQ(window.getWidth(), 320);
    EXPECT_EQ(window.getHeight(), 240);
    EXPECT_NE(window.getNativeHandle(), nullptr);
}

// ======== Move semantics ======== //

TEST_F(GLTest, MoveConstructorTransfersTheNativeWindow)
{
    Window source;
    void* sourceHandle = source.getNativeHandle();
    ASSERT_NE(sourceHandle, nullptr);

    Window target{std::move(source)};

    EXPECT_EQ(target.getNativeHandle(), sourceHandle);
    EXPECT_EQ(source.getNativeHandle(), nullptr) << "moving must leave no native handle behind";
    EXPECT_FALSE(target.shouldClose());
}

TEST_F(GLTest, MovedFromWindowIsSafeToDestroyAndAssign)
{
    Window source;
    void* sourceHandle = source.getNativeHandle();

    Window target{std::move(source)};
    // source is moved-from: it may only be destroyed or assigned to.
    source = Window{};
    EXPECT_NE(source.getNativeHandle(), nullptr);
    EXPECT_NE(source.getNativeHandle(), sourceHandle);

    (void)target;
}

TEST_F(GLTest, MoveAssignmentTakesOverTheSourcesWindow)
{
    Window source;
    void* sourceHandle = source.getNativeHandle();
    ASSERT_NE(sourceHandle, nullptr);

    Window target;
    target = std::move(source);

    EXPECT_EQ(target.getNativeHandle(), sourceHandle);
    EXPECT_EQ(source.getNativeHandle(), nullptr);
    EXPECT_FALSE(target.shouldClose());
    expectNoGLError();
}

// ======== Event queue ======== //

TEST_F(GLTest, PollEventReturnsFalseOnAnEmptyQueue)
{
    Window window;
    Event event{};

    EXPECT_FALSE(window.pollEvent(event));
}

TEST_F(GLTest, PushedEventIsPoppedInOrder)
{
    Window window;

    Event first{};
    first.type = EventType::Closed;

    Event second{};
    second.type = EventType::Resized;
    second.size = {640, 480};

    window.pushEvent(first);
    window.pushEvent(second);

    Event popped{};
    ASSERT_TRUE(window.pollEvent(popped));
    EXPECT_EQ(popped.type, EventType::Closed);

    ASSERT_TRUE(window.pollEvent(popped));
    EXPECT_EQ(popped.type, EventType::Resized);
    EXPECT_EQ(popped.size.width, 640);
    EXPECT_EQ(popped.size.height, 480);

    EXPECT_FALSE(window.pollEvent(popped)) << "the queue must be drained after both pops";
}

TEST_F(GLTest, ResizeCallbackQueuesResizedEventAndUpdatesSize)
{
    Window window;

    // Hidden windows still receive framebuffer-size callbacks once events are polled.
    glfwSetWindowSize(static_cast<GLFWwindow*>(window.getNativeHandle()), 321, 123);
    Window::pollEvents();

    Event event{};
    bool sawResize = false;
    while (window.pollEvent(event))
    {
        if (event.type == EventType::Resized)
        {
            sawResize = true;
            EXPECT_EQ(event.size.width, 321);
            EXPECT_EQ(event.size.height, 123);
        }
    }

    EXPECT_TRUE(sawResize) << "a framebuffer resize must produce EventType::Resized";
    EXPECT_EQ(window.getWidth(), 321);
    EXPECT_EQ(window.getHeight(), 123);
}

// ======== Frame lifecycle ======== //

TEST_F(GLTest, BeginFrameClearsTheBoundTargetToTheRequestedColor)
{
    Window window;
    auto target = sogtest::OffscreenTarget::create(8, 8);
    target.bind();

    window.beginFrame(ClearBuffer::Color, Color{0.f, 255.f, 0.f, 255.f});

    const Color pixel = target.readPixel(4, 4);
    sogtest::expectPixelNear(pixel, 0, 255, 0, 255);
    FrameBuffer::unbind();
}

TEST_F(GLTest, BeginFrameDefaultColorIsOpaqueBlack)
{
    Window window;
    auto target = sogtest::OffscreenTarget::create(8, 8);
    target.bind();

    window.beginFrame(ClearBuffer::Color);

    const Color pixel = target.readPixel(4, 4);
    sogtest::expectPixelNear(pixel, 0, 0, 0, 255);
    FrameBuffer::unbind();
}

TEST_F(GLTest, StaticClearWithColorClearsToBlackTransparentAlpha)
{
    Window window;
    auto target = sogtest::OffscreenTarget::create(8, 8);
    target.bind();

    // Unlike beginFrame's 0-255 default, the one-argument clear uses alpha 0.
    Window::clear(ClearBuffer::Color);

    const Color pixel = target.readPixel(4, 4);
    sogtest::expectPixelNear(pixel, 0, 0, 0, 0);
    FrameBuffer::unbind();
}

TEST_F(GLTest, StaticClearWithExplicitColor)
{
    Window window;
    auto target = sogtest::OffscreenTarget::create(8, 8);
    target.bind();

    Window::clear(ClearBuffer::Color | ClearBuffer::Depth, Color{255.f, 0.f, 0.f, 255.f});

    const Color pixel = target.readPixel(4, 4);
    sogtest::expectPixelNear(pixel, 255, 0, 0, 255);
    FrameBuffer::unbind();
}

TEST_F(GLTest, EndFrameSwapsAndPollsWithoutError)
{
    Window window;

    window.beginFrame(ClearBuffer::Color | ClearBuffer::Depth);
    window.endFrame();

    EXPECT_FALSE(window.shouldClose());
    expectNoGLError();
}

// ======== Window::draw ======== //

TEST_F(GLTest, DrawNonIndexedRasterizesTheGeometry)
{
    // A Window must exist first so its context backs these GL objects; declared before them so it
    // is destroyed last.
    Window window;

    const auto vertices = sogtest::fullScreenQuad({1.f, 0.f, 0.f, 1.f});
    VertexBuffer vertexBuffer;
    vertexBuffer.set(std::span<const sog::Vertex>{vertices});

    VertexArray vertexArray;
    vertexArray.create();
    vertexArray.setVertexBuffer(vertexBuffer, 0, sog::Vertex::stride());
    vertexArray.setAttributes(sog::Vertex::attributes());

    const sog::VertexShader vertex{sogtest::COLOR_VERT};
    const sog::FragmentShader fragment{sogtest::VERTEX_COLOR_FRAG};
    ShaderProgram program{vertex, fragment};

    auto target = sogtest::OffscreenTarget::create(32, 32);
    target.bind();
    Window::clear(ClearBuffer::Color, Color{0.f, 0.f, 0.f, 255.f});

    const RenderState state{vertexBuffer, vertexArray, nullptr, program};
    Window::draw(state);

    sogtest::expectPixelNear(target.readPixel(16, 16), 255, 0, 0, 255);
    FrameBuffer::unbind();
    expectNoGLError();
}

TEST_F(GLTest, DrawIndexedRasterizesThroughTheIndexBuffer)
{
    Window window;

    // Two triangles: vertices 0-2 red (full screen), vertices 3-5 blue (full screen).
    auto coloredVertices = sogtest::fullScreenQuad({1.f, 0.f, 0.f, 1.f});
    const auto blueQuad = sogtest::fullScreenQuad({0.f, 0.f, 1.f, 1.f});
    coloredVertices.insert(coloredVertices.end(), blueQuad.begin(), blueQuad.end());

    VertexBuffer vertexBuffer;
    vertexBuffer.set(std::span<const sog::Vertex>{coloredVertices});

    // Only the blue quad's first triangle (indices 6, 7, 8: the lower-right half) is drawn.
    IndexBuffer indexBuffer;
    indexBuffer.set(std::span<const Index>{std::array<Index, 3>{6, 7, 8}});

    VertexArray vertexArray;
    vertexArray.create();
    vertexArray.setVertexBuffer(vertexBuffer, 0, sog::Vertex::stride());
    vertexArray.setAttributes(sog::Vertex::attributes());
    vertexArray.setIndexBuffer(indexBuffer);
    ASSERT_TRUE(vertexArray.indexed());

    const sog::VertexShader vertex{sogtest::COLOR_VERT};
    const sog::FragmentShader fragment{sogtest::VERTEX_COLOR_FRAG};
    ShaderProgram program{vertex, fragment};

    auto target = sogtest::OffscreenTarget::create(32, 32);
    target.bind();
    Window::clear(ClearBuffer::Color, Color{0.f, 0.f, 0.f, 255.f});

    const RenderState state{vertexBuffer, vertexArray, nullptr, program};
    Window::draw(state);

    // Sample inside the drawn lower-right half-triangle (the quad's diagonal crosses the exact
    // center, which a single triangle does not unambiguously cover).
    sogtest::expectPixelNear(target.readPixel(24, 8), 0, 0, 255, 255);
    FrameBuffer::unbind();
    expectNoGLError();
}

TEST_F(GLTest, DrawBindsTheRenderStateTextureToUnitZero)
{
    Window window;

    const auto vertices = sogtest::fullScreenQuad({1.f, 1.f, 1.f, 1.f});
    VertexBuffer vertexBuffer;
    vertexBuffer.set(std::span<const sog::Vertex>{vertices});

    VertexArray vertexArray;
    vertexArray.create();
    vertexArray.setVertexBuffer(vertexBuffer, 0, sog::Vertex::stride());
    vertexArray.setAttributes(sog::Vertex::attributes());

    const sog::VertexShader vertex{sogtest::PASS_THROUGH_VERT};
    const sog::FragmentShader fragment{sogtest::TEXTURE_FRAG};
    ShaderProgram program{vertex, fragment};

    // A 1x1 green texture sampled by the fragment shader proves the texture was bound.
    const std::array<std::uint8_t, 4> greenPixel{0, 255, 0, 255};
    Texture2D texture{greenPixel.data(), 1, 1, sog::gfx::texture_internal_format::RGBA8,
                      sog::gfx::pixel_format::RGBA, sog::gfx::pixel_type::UnsignedByte,
                      sog::gfx::TextureSettings{}};
    const auto textureLocation = program.getLocation("uTexture");
    ASSERT_TRUE(textureLocation.has_value()) << "TEXTURE_FRAG samples uTexture: it must be active";
    sog::uniform::setInt(program, *textureLocation, 0);

    auto target = sogtest::OffscreenTarget::create(32, 32);
    target.bind();
    Window::clear(ClearBuffer::Color, Color{0.f, 0.f, 0.f, 255.f});

    const RenderState state{vertexBuffer, vertexArray, &texture, program};
    Window::draw(state);

    sogtest::expectPixelNear(target.readPixel(16, 16), 0, 255, 0, 255);
    FrameBuffer::unbind();
    expectNoGLError();
}
