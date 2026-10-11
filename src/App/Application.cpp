#include "Application.hpp"

#include "../../cmake-build-debug/_deps/glfw-src/include/GLFW/glfw3.h"
#include "../Core/gfx/OpenGL/Shader.hpp"
#include "../Core/gfx/OpenGL/Uniforms.hpp"
#include "Input.hpp"
#include "gfx/Camera.hpp"
#include "gfx/Indices.hpp"
#include "gfx/OpenGL/ShaderProgram.hpp"
#include "gfx/OpenGL/VertexArray.hpp"
#include "gfx/Window/Window.hpp"

namespace sog
{

Application::Application() = default;
int Application::Run()
{
    gfx::Window window;

    std::vector vertices = {
        gfx::Vertex{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        gfx::Vertex{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        gfx::Vertex{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        gfx::Vertex{{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    };

    std::vector indices = {
        gfx::Index{0}, gfx::Index{1}, gfx::Index{2}, gfx::Index{2}, gfx::Index{3}, gfx::Index{0},
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
    while (!window.shouldClose())
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        gfx::Window::clear(gfx::Color{255, 0, 0, 0});
        input::update(window);
        input::updateCamera(camera, window, deltaTime);
        gfx::uniform::setMat4(shaderProgram, locMVP, camera.getViewProjection() * transform);
        gfx::Window::draw(gfx::RenderState{vertexBuffer, vertexArray, &texture, shaderProgram});

        window.swapBuffers();
        gfx::Window::pollEvents();
    }
    return 0;
}
} // namespace sog