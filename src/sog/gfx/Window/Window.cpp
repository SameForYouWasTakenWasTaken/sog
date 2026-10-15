/**
 * @file Window.cpp
 * @brief Implementation of gfx::Window: GLFW window/context creation, the frame lifecycle, the
 * engine event queue and the static draw helper.
 */

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <sog/Error/Exceptions/graphics_error.hpp>
#include <sog/Utility/TypeTraits.hpp>
#include <sog/gfx/OpenGL/Common.hpp>
#include <sog/gfx/OpenGL/Objects/ShaderProgram.hpp>
#include <sog/gfx/OpenGL/Objects/VertexArray.hpp>
#include <sog/gfx/Window/Window.hpp>

namespace sog::gfx::backend
{
/**
 * @brief Translates an engine OpenGL profile into the corresponding GLFW constant.
 * @param profile Engine profile (CORE or COMPATIBILITY).
 * @return GLFW_OPENGL_CORE_PROFILE or GLFW_OPENGL_COMPAT_PROFILE.
 * @throws std::invalid_argument If the profile has no GLFW mapping.
 */
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

/**
 * @brief Private pimpl holding the native GLFWwindow and its resize callback.
 *
 * The resize callback (on_resize) is registered via
 * glfwSetFramebufferSizeCallback; it recovers the owning Window from the GLFW
 * user pointer, updates the stored dimensions, resizes the GL viewport and
 * enqueues an EventType::Resized event (ignored while minimized, i.e. size 0).
 */
struct Window::ImplWindow
{
    GLFWwindow* glfwHandle = nullptr; /**< Native GLFW window; null until initialize() succeeds. */

    /**
     * @brief GLFW framebuffer-size callback: updates size, viewport and queues a Resized event.
     * @param window GLFWwindow whose size changed.
     * @param width New framebuffer width in pixels.
     * @param height New framebuffer height in pixels.
     */
    static void on_resize(GLFWwindow* window, int width, int height)
    {
        auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));

        if (!self)
            return;

        self->m_Context.width = width;
        self->m_Context.height = height;
        common::ViewportToActiveTarget(0, 0, width, height);

        if (window && width > 0 && height > 0)
        {
            Event e{};
            e.type = EventType::Resized;
            e.size = {width, height};
            self->pushEvent(e);
        }
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
Window::Window(const context::WindowContext& context)
    : m_Window(std::make_unique<ImplWindow>(nullptr))
{
    initialize(context);
}
bool Window::shouldClose() const { return glfwWindowShouldClose(m_Window->glfwHandle); }
void Window::swapBuffers() const { glfwSwapBuffers(m_Window->glfwHandle); }
void Window::beginFrame(ClearBuffer buffers, Color clearColor) { clear(buffers, clearColor); }
void Window::endFrame() const
{
    swapBuffers();
    pollEvents();
}
void Window::pollEvents() { glfwPollEvents(); }
void Window::clear(ClearBuffer buffers, Color color)
{
    auto normalizedColor = Color::fromRGBA(color.r, color.g, color.b, color.a);
    glClearColor(normalizedColor.r, normalizedColor.g, normalizedColor.b, normalizedColor.a);
    glClear(to_underlying(buffers));
}
void Window::clear(ClearBuffer buffers) { clear(buffers, Color{0, 0, 0, 0}); }
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
        common::DrawElements(draw_mode::TRIANGLES,
                             static_cast<GLsizei>(state.vertexArray.indexCount()), index_type::UINT,
                             nullptr);

        return;
    }
    // non indexed
    common::DrawArrays(draw_mode::TRIANGLES, 0, static_cast<GLsizei>(state.vertexBuffer.size()));
}
void* Window::getNativeHandle() const { return m_Window->glfwHandle; }
bool Window::pollEvent(Event& outEvent)
{
    if (m_EventQueue.empty())
        return false;

    outEvent = m_EventQueue.front();
    m_EventQueue.pop();
    return true;
}
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
        // Constructor is about to throw: clean up the already-created window so it is not leaked
        // (~Window never runs for a failed construction).
        glfwDestroyWindow(m_Window->glfwHandle);
        m_Window->glfwHandle = nullptr;
        glfwTerminate();
        throw graphics_error(graphics_error_code::window_creation_failed,
                             "Failed to create window! (glad did not initialize properly)");
    }
}
} // namespace sog::gfx