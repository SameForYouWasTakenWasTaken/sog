#include "Application.hpp"

#include "../../cmake-build-debug/_deps/glfw-src/include/GLFW/glfw3.h"
#include "../Core/Utility/Uniforms.hpp"
#include "../Core/gfx/OpenGL/Objects/Shader.hpp"
#include "../Core/gfx/OpenGL/Objects/ShaderProgram.hpp"
#include "../Core/gfx/OpenGL/Objects/VertexArray.hpp"
#include "Input.hpp"
#include "Utility/Time.hpp"
#include "gfx/Camera.hpp"
#include "gfx/Indices.hpp"
#include "gfx/OpenGL/Objects/FrameBuffer.hpp"
#include "gfx/OpenGL/SetRenderingState.hpp"
#include "gfx/Window/Window.hpp"

namespace sog
{

Application::Application() = default;
int Application::Run()
{
    gfx::Window window;

    std::vector vertices = {
        // Front face (z = 0.5)
        gfx::Vertex{{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        gfx::Vertex{{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        gfx::Vertex{{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        gfx::Vertex{{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},

        // Back face (z = -0.5)
        gfx::Vertex{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
        gfx::Vertex{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
        gfx::Vertex{{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        gfx::Vertex{{-0.5f, 0.5f, -0.5f}, {0.2f, 0.2f, 0.2f, 1.0f}, {0.0f, 1.0f}},
    };

    std::vector indices = {
        // Front
        gfx::Index{0},
        gfx::Index{1},
        gfx::Index{2},
        gfx::Index{2},
        gfx::Index{3},
        gfx::Index{0},

        // Back
        gfx::Index{5},
        gfx::Index{4},
        gfx::Index{7},
        gfx::Index{7},
        gfx::Index{6},
        gfx::Index{5},

        // Left
        gfx::Index{4},
        gfx::Index{0},
        gfx::Index{3},
        gfx::Index{3},
        gfx::Index{7},
        gfx::Index{4},

        // Right
        gfx::Index{1},
        gfx::Index{5},
        gfx::Index{6},
        gfx::Index{6},
        gfx::Index{2},
        gfx::Index{1},

        // Top
        gfx::Index{3},
        gfx::Index{2},
        gfx::Index{6},
        gfx::Index{6},
        gfx::Index{7},
        gfx::Index{3},

        // Bottom
        gfx::Index{4},
        gfx::Index{5},
        gfx::Index{1},
        gfx::Index{1},
        gfx::Index{0},
        gfx::Index{4},
    };
    gfx::VertexBuffer vertexBuffer;
    vertexBuffer.set(vertices);

    gfx::IndexBuffer elementBuffer;
    elementBuffer.set(indices);

    gfx::VertexArray vertexArray;
    vertexArray.create();
    vertexArray.setVertexBuffer(vertexBuffer, 0, gfx::Vertex::stride());
    vertexArray.setIndexBuffer(elementBuffer);
    vertexArray.setAttributes(gfx::Vertex::attributes());

    gfx::VertexShader vertexShader;
    vertexShader.createFromFile("assets/shaders/vertex.glsl");
    gfx::FragmentShader fragment;
    fragment.createFromFile("assets/shaders/fragment.glsl");

    gfx::Texture2D texture;
    texture.createFromFile("assets/Textures/test.jpg", {.wrapS = gfx::texture_params::REPEAT,
                                                        .wrapT = gfx::texture_params::REPEAT});

    gfx::ShaderProgram shaderProgram;
    shaderProgram.Initialize(vertexShader, fragment);

    auto locTexture = gfx::uniform::getLocation(shaderProgram, "texture2D");
    gfx::uniform::setInt(shaderProgram, locTexture, 0);

    auto transform = glm::mat4(1.0f);
    transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    gfx::Camera camera;

    auto locMVP = gfx::uniform::getLocation(shaderProgram, "MVP");

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    gfx::SetRenderingState(gfx::RenderSettings{.depthTest = true, .blending = true});

    gfx::RenderBuffer depthStencilBuffer(GL_DEPTH24_STENCIL8, 800, 600);
    auto colorTexture = gfx::Texture2D::empty(800, 600);

    gfx::FrameBuffer frameBuffer;
    frameBuffer.attachTexture(gfx::FramebufferAttachment::Color0, colorTexture.getHandle());
    frameBuffer.attachRenderbuffer(gfx::FramebufferAttachment::DepthStencil, depthStencilBuffer);

    gfx::VertexShader frameBufVertShader;
    frameBufVertShader.createFromFile("assets/shaders/PPO.vert");
    gfx::FragmentShader frameBufFragShader;
    frameBufFragShader.createFromFile("assets/shaders/PPO.frag");

    gfx::ShaderProgram frameBufShaderProgram;
    frameBufShaderProgram.Initialize(frameBufVertShader, frameBufFragShader);

    auto locFramebufTexture = frameBufShaderProgram.getLocation("screenTexture");
    auto locFramebufTime = frameBufShaderProgram.getLocation("time");
    auto locFramebufIntensity = frameBufShaderProgram.getLocation("intensity");
    gfx::uniform::setInt(frameBufShaderProgram, locFramebufTexture.value_or(gfx::Uniform{}), 0);

    assert(frameBuffer.isComplete());
    while (!window.shouldClose())
    {
        auto currentFrame = static_cast<float>(Time::getTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        gfx::Event e{};
        while (window.pollEvent(e))
        {
            if (e.type == gfx::EventType::Resized)
            {
                int width = e.size.width;
                int height = e.size.height;

                // Resize your FBO resources seamlessly right here!
                depthStencilBuffer = gfx::RenderBuffer(GL_DEPTH24_STENCIL8, width, height);
                colorTexture = gfx::Texture2D{};
                colorTexture.createFromRaw(nullptr, width, height, GL_RGBA8, GL_RGBA,
                                           GL_UNSIGNED_BYTE, {});

                frameBuffer.attachTexture(gfx::FramebufferAttachment::Color0,
                                          colorTexture.getHandle());
                frameBuffer.attachRenderbuffer(gfx::FramebufferAttachment::DepthStencil,
                                               depthStencilBuffer);
            }
        }

        input::update(window);
        input::updateCamera(camera, window, deltaTime);

        frameBuffer.bind();
        glViewport(0, 0, window.getWidth(), window.getHeight());
        gfx::Window::clear(gfx::ClearBuffer::Color | gfx::ClearBuffer::Depth,
                           gfx::Color{255.f, 255.f, 255.f, 255.f});

        // drawing
        gfx::uniform::setMat4(shaderProgram, locMVP, camera.getViewProjection() * transform);
        gfx::Window::draw(gfx::RenderState{vertexBuffer, vertexArray, &texture, shaderProgram});

        // framebuffer
        gfx::uniform::runSafe(
            [&]
            {
                gfx::uniform::setFloat(frameBufShaderProgram,
                                       locFramebufTime.value_or(gfx::Uniform{}), currentFrame);
            },
            [&]
            {
                gfx::uniform::setFloat(frameBufShaderProgram,
                                       locFramebufIntensity.value_or(gfx::Uniform{}), 0.2f);
            });
        // apply post processing
        gfx::FrameBuffer::unbind();
        window.beginFrame(gfx::ClearBuffer::Color | gfx::ClearBuffer::Depth,
                          gfx::Color{255, 0, 0, 0});
        frameBufShaderProgram.use();
        colorTexture.bind(0);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        window.endFrame();
    }
    return 0;
}
} // namespace sog