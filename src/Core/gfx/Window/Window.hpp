/**
 * @file Window.hpp
 * @brief Defines gfx::Window, the GLFW-backed OS window with an OpenGL context, frame lifecycle, event queue and a static draw helper.
 */

#pragma once
#include <memory>
#include <queue>
#include <string>

#include "ClearBuffer.hpp"
#include "Event.hpp"
#include "RenderState.hpp"
#include "WindowContext.hpp"
#include "gfx/Types/Colors.hpp"
#include "glad/gl.h"

namespace sog::gfx
{
/**
 * @brief Owns a GLFW window and its OpenGL context, and drives the per-frame render loop.
 *
 * Construction initializes GLFW (throwing sog::graphics_error on failure), creates
 * the window with the requested context::WindowContext, makes the context current
 * and loads GL entry points via glad. Destruction destroys the GLFW window.
 *
 * Typical frame loop:
 * @code
 * while (!window.shouldClose())
 * {
 *     window.beginFrame(ClearBuffer::Color | ClearBuffer::Depth, {0, 0, 0, 255});
 *     Window::draw(state);
 *     window.endFrame();
 * }
 * @endcode
 *
 * beginFrame() clears the requested buffers with the given color; endFrame()
 * swaps the buffers and polls native events. The engine's own Event queue is fed
 * by callbacks (e.g. resize) and drained with pollEvent().
 *
 * @attention beginFrame()'s clear color is interpreted as 8-bit-per-channel
 * (0-255) floats, matching the {0, 0, 0, 255} default — a normalized [0, 1] Color
 * would be divided by 255 again by the internal fromRGBA() conversion.
 *
 * @see sog::context::WindowContext, sog::gfx::RenderState
 */
class Window
{
  public:
    /**
     * @brief Creates a window with the default context::WindowContext (4.6 core, 800x600).
     * @throws sog::graphics_error If GLFW or GLAD fails to initialize.
     */
    Window();

    /**
     * @brief Destroys the GLFW window and releases its resources.
     */
    ~Window();

    /**
     * @brief Creates a window with an explicit context configuration.
     * @param context Requested OpenGL version/profile, size and title.
     * @throws sog::graphics_error If window or context creation fails.
     */
    Window(const context::WindowContext& context);

    /**
     * @brief Reports whether the OS has requested the window to close.
     * @return true once the user asks to close the window.
     */
    [[nodiscard]] bool shouldClose() const;

    /**
     * @brief Swaps the front and back buffers (presents the rendered frame).
     */
    void swapBuffers() const;

    /**
     * @brief Starts a frame by clearing the given buffers with the given color.
     * @param buffers Which buffer(s) to clear; combine with operator|.
     * @param clearColor Clear color in 0-255 per channel (see class @attention).
     */
    void beginFrame(ClearBuffer buffers, Color clearColor = {0, 0, 0, 255});

    /**
     * @brief Ends a frame: swaps buffers, then polls native OS events.
     */
    void endFrame() const;

    /**
     * @brief Polls native (GLFW) events, dispatching registered callbacks such as resize.
     */
    static void pollEvents();

    /**
     * @brief Clears the given buffers using the given color.
     * @param buffers Which buffer(s) to clear; combine with operator|.
     * @param color Clear color in 0-255 per channel (see class @attention).
     */
    static void clear(ClearBuffer buffers, Color color);

    /**
     * @brief Clears the given buffers with an opaque black color.
     * @param buffers Which buffer(s) to clear; combine with operator|.
     */
    static void clear(ClearBuffer buffers);

    /**
     * @brief Binds the state and issues the draw call (indexed or non-indexed).
     * @param state Fully populated RenderState; all referenced objects must be alive.
     *
     * Uses glDrawElements when the vertex array has an index buffer, otherwise
     * glDrawArrays over the vertex buffer's element count. The texture is bound to
     * unit 0 only when state.texture2D is non-null.
     */
    static void draw(const RenderState& state);

    /**
     * @brief Returns the current window width in pixels.
     * @return Width stored in the window context (updated on resize).
     */
    [[nodiscard]] int getWidth() const { return m_Context.width; }

    /**
     * @brief Returns the current window height in pixels.
     * @return Height stored in the window context (updated on resize).
     */
    [[nodiscard]] int getHeight() const { return m_Context.height; }

    /**
     * @brief Returns the underlying native handle for interop.
     * @return Pointer to the GLFWwindow (opaque to callers).
     */
    [[nodiscard]] void* getNativeHandle() const;

    /**
     * @brief Pops the next queued engine event, if any.
     * @param outEvent Receives the popped event on success; unmodified when the queue is empty.
     * @return true if an event was popped, false if the queue was empty.
     */
    bool pollEvent(Event& outEvent);

    /**
     * @brief Enqueues an engine event for later retrieval by pollEvent().
     * @param event The event to queue (e.g. from a callback).
     */
    void pushEvent(const Event& event) { m_EventQueue.push(event); }

  private:
    std::queue<Event> m_EventQueue; /**< Engine-level events waiting to be consumed by pollEvent(). */
    void initialize(const context::WindowContext& context); /**< Sets up GLFW, the window, context and GLAD; throws on failure. */

    context::WindowContext m_Context; /**< The context settings this window was created with (tracks current size). */

    struct ImplWindow; /**< Forward-declared private implementation holding the GLFWwindow*. */

    std::shared_ptr<ImplWindow> m_Window; /**< Shared ownership of the pimpl (GLFWwindow wrapper). */
};
} // namespace sog::gfx
