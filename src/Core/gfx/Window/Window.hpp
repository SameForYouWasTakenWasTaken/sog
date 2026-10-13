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
class Window
{
  public:
    Window();
    ~Window();
    Window(const context::WindowContext& context);

    [[nodiscard]] bool shouldClose() const;
    void swapBuffers() const;

    void beginFrame(ClearBuffer buffers, Color clearColor = {0, 0, 0, 255});
    void endFrame() const;
    static void pollEvents();
    static void clear(ClearBuffer buffers, Color color);
    static void clear(ClearBuffer buffers);
    static void draw(const RenderState& state);

    [[nodiscard]] int getWidth() const { return m_Context.width; }
    [[nodiscard]] int getHeight() const { return m_Context.height; }

    [[nodiscard]] void* getNativeHandle() const;

    bool pollEvent(Event& outEvent);
    void pushEvent(const Event& event) { m_EventQueue.push(event); }

  private:
    std::queue<Event> m_EventQueue;
    void initialize(const context::WindowContext& context);

    context::WindowContext m_Context;

    struct ImplWindow;

    std::shared_ptr<ImplWindow> m_Window;
};
} // namespace sog::gfx