#include "Window.hpp"

#include <GLFW/glfw3.h>
#include <complex>
#include <cstdio>

#include "../OpenGL/ShaderProgram.hpp"
#include "../OpenGL/VertexArray.hpp"
#include "Error/Exceptions/graphics_error.hpp"
#include "Util.hpp"

namespace sog::gfx::backend
{
int toGLFWProfile(context::WindowContext::OpenGLProfile profile)
{
    switch (profile)
    {
    case context::WindowContext::OpenGLProfile::CORE:
        return GLFW_OPENGL_CORE_PROFILE;

    case context::WindowContext::OpenGLProfile::COMPATIBILITY:
        return GLFW_OPENGL_COMPAT_PROFILE;
    }

    throw std::invalid_argument("Invalid OpenGL profile");
}
} // namespace sog::gfx::backend

namespace sog::gfx
{
namespace
{
const char* debugSourceToString(GLenum source)
{
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        return "API";

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        return "WINDOW_SYSTEM";

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        return "SHADER_COMPILER";

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        return "THIRD_PARTY";

    case GL_DEBUG_SOURCE_APPLICATION:
        return "APPLICATION";

    case GL_DEBUG_SOURCE_OTHER:
        return "OTHER";

    default:
        return "UNKNOWN";
    }
}

const char* debugTypeToString(GLenum type)
{
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        return "ERROR";

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        return "DEPRECATED_BEHAVIOR";

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        return "UNDEFINED_BEHAVIOR";

    case GL_DEBUG_TYPE_PORTABILITY:
        return "PORTABILITY";

    case GL_DEBUG_TYPE_PERFORMANCE:
        return "PERFORMANCE";

    case GL_DEBUG_TYPE_MARKER:
        return "MARKER";

    case GL_DEBUG_TYPE_PUSH_GROUP:
        return "PUSH_GROUP";

    case GL_DEBUG_TYPE_POP_GROUP:
        return "POP_GROUP";

    case GL_DEBUG_TYPE_OTHER:
        return "OTHER";

    default:
        return "UNKNOWN";
    }
}

const char* debugSeverityToString(GLenum severity)
{
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        return "HIGH";

    case GL_DEBUG_SEVERITY_MEDIUM:
        return "MEDIUM";

    case GL_DEBUG_SEVERITY_LOW:
        return "LOW";

    case GL_DEBUG_SEVERITY_NOTIFICATION:
        return "NOTIFICATION";

    default:
        return "UNKNOWN";
    }
}

void GLAD_API_PTR onDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity,
                                 GLsizei /*length*/, const GLchar* message,
                                 const void* /*userParam*/)
{
    std::fprintf(stderr, "[OpenGL][%s][%s][%s][%u] %s\n", debugSourceToString(source),
                 debugTypeToString(type), debugSeverityToString(severity), id, message);
}
} // namespace

struct Window::ImplWindow
{
    GLFWwindow* glfwHandle = nullptr;

    static void on_resize(GLFWwindow* window, int width, int height)
    {
        auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));

        if (!self)
            return;

        self->m_Context.width = width;
        self->m_Context.height = height;

        glViewport(0, 0, width, height);
    }
};
Window::Window() : m_Window(std::make_unique<ImplWindow>(nullptr))
{
    initialize(context::WindowContext{});
}
Window::~Window()
{
    if (m_Window->glfwHandle)
        glfwDestroyWindow(m_Window->glfwHandle);
}
Window::Window(const context::WindowContext& context) { initialize(context); }
bool Window::shouldClose() const { return glfwWindowShouldClose(m_Window->glfwHandle); }
void Window::swapBuffers() const { glfwSwapBuffers(m_Window->glfwHandle); }
void Window::pollEvents() { glfwPollEvents(); }
void Window::clear(Color color)
{
    auto normalizedColor = Color::fromRGBA(color.r, color.g, color.b, color.a);
    glClearColor(normalizedColor.r, normalizedColor.g, normalizedColor.b, normalizedColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
}
void Window::clear() { clear(Color{0, 0, 0, 0}); }
void Window::draw(const RenderState& state)
{
    state.shaderProgram.use();
    state.vertexArray.bind();
    if (state.texture2D)
    {
        state.texture2D->bind(0);
    }
    // indexed
    if (state.vertexArray.indexed())
    {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(state.vertexArray.indexCount()),
                       GL_UNSIGNED_INT, nullptr);

        return;
    }
    // non indexed
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(state.vertexBuffer.size()));
}
void* Window::getNativeHandle() const { return m_Window->glfwHandle; }
void Window::initialize(const context::WindowContext& context)
{
    if (!glfwInit())
        throw graphics_error(graphics_error_code::window_creation_failed,
                             "Failed to create window! (glfw did not initialize properly)");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, context.OpenGL_Version_Major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, context.OpenGL_Version_Minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, backend::toGLFWProfile(context.OpenGL_Profile));
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    m_Window->glfwHandle =
        glfwCreateWindow(context.width, context.height, context.title.c_str(), nullptr, nullptr);
    if (!m_Window->glfwHandle)
    {
        glfwTerminate();
        throw graphics_error(graphics_error_code::window_creation_failed,
                             "Failed to create window!");
    }
    glfwMakeContextCurrent(m_Window->glfwHandle);
    glfwSetWindowUserPointer(m_Window->glfwHandle, this);
    glfwSetFramebufferSizeCallback(m_Window->glfwHandle, ImplWindow::on_resize);

    if (!gladLoadGL(glfwGetProcAddress))
    {
        throw graphics_error(graphics_error_code::window_creation_failed,
                             "Failed to create window! (glad did not initialize properly)");
    }

    // Route every GL diagnostic to stderr so silent GL_INVALID_* calls show up.
    // SYNCHRONOUS keeps the callback on the calling thread, so the message lands
    // right next to the line that caused it.
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(onDebugMessage, nullptr);
}
} // namespace sog::gfx