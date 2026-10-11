#pragma once
#include <memory>
#include <string>

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

    static void pollEvents();
    static void clear(Color color);
    static void clear();
    static void draw(const RenderState& state);

    [[nodiscard]] int getWidth() const { return m_Context.width; }
    [[nodiscard]] int getHeight() const { return m_Context.height; }

    [[nodiscard]] void* getNativeHandle() const;

  private:
    void initialize(const context::WindowContext& context);

    context::WindowContext m_Context;

    struct ImplWindow;

    std::shared_ptr<ImplWindow> m_Window;
};
} // namespace sog::gfx