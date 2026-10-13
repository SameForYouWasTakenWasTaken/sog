#pragma once
#include <cstdint>
#include <utility>

#include "../Types/Handles.hpp"
#include "gfx/Types/Handles.hpp"
#include "glad/gl.h"

namespace sog::gfx
{

class RenderBuffer
{
  public:
    RenderBuffer() = default;

    // Create a renderbuffer with specific internal format, width, and height
    RenderBuffer(GLenum internalFormat, GLsizei width, GLsizei height)
    {
        create(internalFormat, width, height);
    }

    ~RenderBuffer() { release(); }

    // Non-copyable
    RenderBuffer(const RenderBuffer&) = delete;
    RenderBuffer& operator=(const RenderBuffer&) = delete;

    // Movable
    RenderBuffer(RenderBuffer&& o) noexcept
        : m_Handle(std::move(o.m_Handle)), m_Width(std::exchange(o.m_Width, 0)),
          m_Height(std::exchange(o.m_Height, 0)),
          m_InternalFormat(std::exchange(o.m_InternalFormat, 0))
    {
    }

    RenderBuffer& operator=(RenderBuffer&& o) noexcept
    {
        if (this != &o)
        {
            release();
            m_Handle = std::move(o.m_Handle);
            m_Width = std::exchange(o.m_Width, 0);
            m_Height = std::exchange(o.m_Height, 0);
            m_InternalFormat = std::exchange(o.m_InternalFormat, 0);
        }
        return *this;
    }

    void create(GLenum internalFormat, GLsizei width, GLsizei height);

    void bind(GLenum target = GL_RENDERBUFFER) const { glBindRenderbuffer(target, m_Handle.get()); }

    static void unbind(GLenum target = GL_RENDERBUFFER) { glBindRenderbuffer(target, 0); }

    void release();

    [[nodiscard]] GLuint id() const { return m_Handle.get(); }
    [[nodiscard]] GLsizei width() const { return m_Width; }
    [[nodiscard]] GLsizei height() const { return m_Height; }
    [[nodiscard]] GLenum internalFormat() const { return m_InternalFormat; }
    [[nodiscard]] bool valid() const { return m_Handle.valid(); }

    [[nodiscard]] explicit operator bool() const { return valid(); }

  private:
    RenderBufferHandle m_Handle{};
    GLsizei m_Width{0};
    GLsizei m_Height{0};
    GLenum m_InternalFormat{0};
};

} // namespace sog::gfx