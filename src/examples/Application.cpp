/**
 * @file Application.cpp
 * @brief Implements sog::Application::Run(), the demo main loop: builds a textured cube, renders it
 * into an offscreen framebuffer, then applies a post-processing pass.
 */

#include "Application.hpp"

#include "sog/gfx/OpenGL/SetRenderingState.hpp"
#include "sog/sog.hpp"
#include "sog/sog_glad.hpp"

/// Default-constructs the Application; no setup is performed here.
Application::Application() = default;

int Application::Run()
{
    sog::Window window;
    std::vector vertices = {
        // Front face (z = 0.5)
        sog::Vertex{{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        sog::Vertex{{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        sog::Vertex{{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        sog::Vertex{{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},

        // Back face (z = -0.5)
        sog::Vertex{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
        sog::Vertex{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
        sog::Vertex{{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        sog::Vertex{{-0.5f, 0.5f, -0.5f}, {0.2f, 0.2f, 0.2f, 1.0f}, {0.0f, 1.0f}},
    };

    std::vector indices = {
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
    sog::VertexBuffer vertexBuffer;
    vertexBuffer.set(vertices);

    sog::IndexBuffer elementBuffer;
    elementBuffer.set(indices);

    sog::VertexArray vertexArray;
    vertexArray.create();
    vertexArray.setVertexBuffer(vertexBuffer, 0, sog::Vertex::stride());
    vertexArray.setIndexBuffer(elementBuffer);
    vertexArray.setAttributes(sog::Vertex::attributes());

    sog::VertexShader vertexShader;
    vertexShader.createFromFile("assets/shaders/vertex.glsl");
    sog::FragmentShader fragment;
    fragment.createFromFile("assets/shaders/fragment.glsl");

    sog::Texture2D texture;
    texture.createFromFile("assets/Textures/test.jpg", {.wrapS = sog::gfx::texture_params::REPEAT,
                                                        .wrapT = sog::gfx::texture_params::REPEAT});

    sog::ShaderProgram shaderProgram;
    shaderProgram.Initialize(vertexShader, fragment);

    auto locTexture = sog::uniform::getLocation(shaderProgram, "texture2D");
    sog::uniform::setInt(shaderProgram, locTexture, 0);

    auto transform = glm::mat4(1.0f);
    transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    sog::Camera camera;

    auto locMVP = sog::uniform::getLocation(shaderProgram, "MVP");

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    sog::gfx::SetRenderingState(sog::gfx::RenderSettings{.depthTest = true, .blending = true});

    sog::RenderBuffer depthStencilBuffer(GL_DEPTH24_STENCIL8, 800, 600);
    auto colorTexture = sog::Texture2D::empty(800, 600);

    sog::FrameBuffer frameBuffer;
    frameBuffer.attachTexture(sog::gfx::FramebufferAttachment::Color0, colorTexture.getHandle());
    frameBuffer.attachRenderbuffer(sog::gfx::FramebufferAttachment::DepthStencil,
                                   depthStencilBuffer);

    sog::VertexShader frameBufVertShader;
    frameBufVertShader.createFromFile("assets/shaders/PPO.vert");
    sog::FragmentShader frameBufFragShader;
    frameBufFragShader.createFromFile("assets/shaders/PPO.frag");

    sog::ShaderProgram frameBufShaderProgram;
    frameBufShaderProgram.Initialize(frameBufVertShader, frameBufFragShader);

    auto locFramebufTexture = frameBufShaderProgram.getLocation("screenTexture");
    auto locFramebufTime = frameBufShaderProgram.getLocation("time");
    auto locFramebufIntensity = frameBufShaderProgram.getLocation("intensity");
    sog::uniform::setInt(frameBufShaderProgram, locFramebufTexture.value_or(sog::Uniform{}), 0);

    assert(frameBuffer.isComplete());
    while (!window.shouldClose())
    {
        auto currentFrame = static_cast<float>(sog::Time::getTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        sog::Event e{};
        while (window.pollEvent(e))
        {
            if (e.type == sog::EventType::Resized)
            {
                int width = e.size.width;
                int height = e.size.height;

                // Resize your FBO resources seamlessly right here!
                depthStencilBuffer = sog::RenderBuffer(GL_DEPTH24_STENCIL8, width, height);
                colorTexture = sog::Texture2D{};
                colorTexture.createFromRaw(nullptr, width, height, GL_RGBA8, GL_RGBA,
                                           GL_UNSIGNED_BYTE, {});

                frameBuffer.attachTexture(sog::gfx::FramebufferAttachment::Color0,
                                          colorTexture.getHandle());
                frameBuffer.attachRenderbuffer(sog::gfx::FramebufferAttachment::DepthStencil,
                                               depthStencilBuffer);
            }
        }

        sog::input::update(window);
        sog::input::updateCamera(camera, window, deltaTime);

        frameBuffer.bind();
        glViewport(0, 0, window.getWidth(), window.getHeight());
        sog::Window::clear(sog::ClearBuffer::Color | sog::ClearBuffer::Depth,
                           sog::Color{255.f, 255.f, 255.f, 255.f});

        // drawing
        sog::uniform::setMat4(shaderProgram, locMVP, camera.getViewProjection() * transform);
        sog::Window::draw(
            sog::gfx::RenderState{vertexBuffer, vertexArray, &texture, shaderProgram});

        // framebuffer
        sog::uniform::runSafe(
            [&]
            {
                sog::uniform::setFloat(frameBufShaderProgram,
                                       locFramebufTime.value_or(sog::Uniform{}), currentFrame);
            },
            [&]
            {
                sog::uniform::setFloat(frameBufShaderProgram,
                                       locFramebufIntensity.value_or(sog::Uniform{}), 0.2f);
            });
        // apply post processing
        sog::FrameBuffer::unbind();
        window.beginFrame(sog::ClearBuffer::Color | sog::ClearBuffer::Depth,
                          sog::Color{255, 0, 0, 0});
        frameBufShaderProgram.use();
        colorTexture.bind(0);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        window.endFrame();
    }
    return 0;
}