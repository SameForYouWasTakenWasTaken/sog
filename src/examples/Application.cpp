/**
 * @file Application.cpp
 * @brief Implements the demo main loop: renders a textured cube into an
 * offscreen framebuffer, then applies a post-processing pass.
 */

#include "Application.hpp"

#include <cassert>
#include <filesystem>
#include <vector>

#include "sog/gfx/OpenGL/SetRenderingState.hpp"
#include "sog/sog.hpp"
#include "sog/sog_glad.hpp"

/// Default-constructs the Application; no setup is performed here.
Application::Application() = default;

namespace
{
struct Object
{
    float width = 1.f;
    float height = 1.f;
    float depth = 1.f;

    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

    std::vector<sog::Vertex> vertices = {
        // Front face
        sog::Vertex{{-0.5f, -0.5f, 0.5f}, {1.f, 0.f, 0.f, 1.f}, {0.f, 0.f}},
        sog::Vertex{{0.5f, -0.5f, 0.5f}, {0.f, 1.f, 0.f, 1.f}, {1.f, 0.f}},
        sog::Vertex{{0.5f, 0.5f, 0.5f}, {0.f, 0.f, 1.f, 1.f}, {1.f, 1.f}},
        sog::Vertex{{-0.5f, 0.5f, 0.5f}, {1.f, 1.f, 0.f, 1.f}, {0.f, 1.f}},

        // Back face
        sog::Vertex{{-0.5f, -0.5f, -0.5f}, {1.f, 0.f, 1.f, 1.f}, {0.f, 0.f}},
        sog::Vertex{{0.5f, -0.5f, -0.5f}, {0.f, 1.f, 1.f, 1.f}, {1.f, 0.f}},
        sog::Vertex{{0.5f, 0.5f, -0.5f}, {1.f, 1.f, 1.f, 1.f}, {1.f, 1.f}},
        sog::Vertex{{-0.5f, 0.5f, -0.5f}, {0.2f, 0.2f, 0.2f, 1.f}, {0.f, 1.f}},
    };

    std::vector<sog::Index> indices = {
        // Front
        sog::Index{0},
        sog::Index{1},
        sog::Index{2},
        sog::Index{2},
        sog::Index{3},
        sog::Index{0},

        // Back
        sog::Index{5},
        sog::Index{4},
        sog::Index{7},
        sog::Index{7},
        sog::Index{6},
        sog::Index{5},

        // Left
        sog::Index{4},
        sog::Index{0},
        sog::Index{3},
        sog::Index{3},
        sog::Index{7},
        sog::Index{4},

        // Right
        sog::Index{1},
        sog::Index{5},
        sog::Index{6},
        sog::Index{6},
        sog::Index{2},
        sog::Index{1},

        // Top
        sog::Index{3},
        sog::Index{2},
        sog::Index{6},
        sog::Index{6},
        sog::Index{7},
        sog::Index{3},

        // Bottom
        sog::Index{4},
        sog::Index{5},
        sog::Index{1},
        sog::Index{1},
        sog::Index{0},
        sog::Index{4},
    };

    [[nodiscard]] sog::math::Mat4 getModelMatrix() const
    {
        sog::math::Mat4 model{1.f};
        model = sog::math::translate(model, sog::Vec3{x, y, z});
        model = sog::math::scale(model, sog::Vec3{width, height, depth});

        return model;
    }
};

struct ObjectBuffers
{
    sog::VertexBuffer vertexBuffer;
    sog::IndexBuffer indexBuffer;
    sog::VertexArray vertexArray;
};

[[nodiscard]] ObjectBuffers createObjectBuffers(const Object& object)
{
    ObjectBuffers buffers;

    buffers.vertexBuffer.set(object.vertices);
    buffers.indexBuffer.set(object.indices);

    buffers.vertexArray.create();
    buffers.vertexArray.setVertexBuffer(buffers.vertexBuffer, 0, sog::Vertex::stride());
    buffers.vertexArray.setIndexBuffer(buffers.indexBuffer);
    buffers.vertexArray.setAttributes(sog::Vertex::attributes());

    return buffers;
}

void prepareObject(const Object& object, sog::ShaderProgram& program, const sog::Texture2D& texture,
                   const sog::math::Mat4& viewProjection)
{
    const auto mvpLocation = sog::uniform::getLocation(program, "MVP");
    const auto textureLocation = sog::uniform::getLocation(program, "texture2D");

    sog::uniform::setMat4(program, mvpLocation, viewProjection * object.getModelMatrix());
    sog::uniform::setInt(program, textureLocation, 0);

    texture.bind(0);
}

void resizeFramebuffer(sog::FrameBuffer& framebuffer, sog::RenderBuffer& depthStencilBuffer,
                       sog::Texture2D& colorTexture, int width, int height)
{
    depthStencilBuffer = sog::RenderBuffer(GL_DEPTH24_STENCIL8, width, height);

    colorTexture = sog::Texture2D{};
    colorTexture.createFromRaw(nullptr, width, height, sog::enums::texture_internal_format::RGB8,
                               sog::enums::pixel_format::RGBA, sog::enums::pixel_type::UnsignedByte,
                               {});

    framebuffer.attachTexture(sog::gfx::FramebufferAttachment::Color0, colorTexture.getHandle());

    framebuffer.attachRenderbuffer(sog::gfx::FramebufferAttachment::DepthStencil,
                                   depthStencilBuffer);
}

void updatePostProcessing(sog::ShaderProgram& program,
                          const std::optional<sog::Uniform>& timeLocation,
                          const std::optional<sog::Uniform>& intensityLocation, float currentFrame)
{
    sog::uniform::runSafe(
        [&]
        { sog::uniform::setFloat(program, timeLocation.value_or(sog::Uniform{}), currentFrame); },
        [&] { sog::uniform::setFloat(program, intensityLocation.value_or(sog::Uniform{}), 0.2f); });
}
} // namespace

int Application::Run()
{
    sog::Window window;

    Object object;
    auto buffers = createObjectBuffers(object);

    sog::VertexShader vertexShader{std::filesystem::path("assets/shaders/vertex.glsl")};
    sog::FragmentShader fragmentShader{std::filesystem::path("assets/shaders/fragment.glsl")};
    sog::ShaderProgram shaderProgram{vertexShader, fragmentShader};

    sog::Texture2D texture;
    texture.createFromFile("assets/Textures/test.jpg", {.wrapS = sog::gfx::texture_params::REPEAT,
                                                        .wrapT = sog::gfx::texture_params::REPEAT});

    object.width = 1.f;
    object.height = 1.f;
    object.depth = 1.f;

    auto transform = glm::mat4{1.f};
    transform = glm::rotate(transform, glm::radians(90.f), glm::vec3{0.f, 0.f, 1.f});

    sog::Camera camera;

    float lastFrame = 0.f;

    sog::gfx::SetRenderingState(sog::gfx::RenderSettings{.depthTest = true, .blending = true});

    sog::RenderBuffer depthStencilBuffer(GL_DEPTH24_STENCIL8, 800, 600);
    auto colorTexture = sog::Texture2D::empty(800, 600);

    sog::FrameBuffer frameBuffer;
    frameBuffer.attachTexture(sog::gfx::FramebufferAttachment::Color0, colorTexture.getHandle());
    frameBuffer.attachRenderbuffer(sog::gfx::FramebufferAttachment::DepthStencil,
                                   depthStencilBuffer);

    sog::VertexShader frameBufVertShader{std::filesystem::path("assets/shaders/PPO.vert")};
    sog::FragmentShader frameBufFragShader{std::filesystem::path("assets/shaders/PPO.frag")};

    sog::ShaderProgram frameBufShaderProgram;
    frameBufShaderProgram.Initialize(frameBufVertShader, frameBufFragShader);

    const auto locFramebufTexture = frameBufShaderProgram.getLocation("screenTexture");
    const auto locFramebufTime = frameBufShaderProgram.getLocation("time");
    const auto locFramebufIntensity = frameBufShaderProgram.getLocation("intensity");

    assert(frameBuffer.isComplete());

    while (!window.shouldClose())
    {
        const auto currentFrame = static_cast<float>(sog::Time::getTime());
        const auto deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        sog::Event event{};
        while (window.pollEvent(event))
        {
            if (event.type == sog::EventType::Resized)
            {
                const int width = event.size.width;
                const int height = event.size.height;

                if (width > 0 && height > 0)
                {
                    resizeFramebuffer(frameBuffer, depthStencilBuffer, colorTexture, width, height);
                }
            }
        }

        sog::input::update(window);
        sog::input::updateCamera(camera, window, deltaTime);

        // Scene pass: render the cube into the framebuffer.
        frameBuffer.bind();

        sog::ViewportToActiveTarget(0, 0, window.getWidth(), window.getHeight());

        sog::Window::clear(sog::ClearBuffer::Color | sog::ClearBuffer::Depth,
                           sog::Color{255.f, 255.f, 255.f, 255.f});

        prepareObject(object, shaderProgram, texture, camera.getViewProjection() * transform);

        sog::Window::draw(sog::gfx::RenderState{buffers.vertexBuffer, buffers.vertexArray, &texture,
                                                shaderProgram});

        // Post-processing pass.
        updatePostProcessing(frameBufShaderProgram, locFramebufTime, locFramebufIntensity,
                             currentFrame);

        sog::FrameBuffer::unbind();

        window.beginFrame(sog::ClearBuffer::Color | sog::ClearBuffer::Depth,
                          sog::Color{255, 0, 0, 0});

        frameBufShaderProgram.use();
        colorTexture.bind(0);

        sog::DrawArrays(sog::enums::draw_mode::TRIANGLES, 0, 3);

        window.endFrame();
    }

    return 0;
}