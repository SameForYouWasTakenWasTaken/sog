#pragma once
#include <cstdint>

#include "gfx/OpenGL/RenderBuffer.hpp"
#include "gfx/Types/Handles.hpp"
#include "glad/gl.h"

namespace sog::gfx
{

enum class FramebufferAttachment
{
    Color0 = GL_COLOR_ATTACHMENT0,
    Color1 = GL_COLOR_ATTACHMENT1,
    Color2 = GL_COLOR_ATTACHMENT2,
    Color3 = GL_COLOR_ATTACHMENT3,
    Depth = GL_DEPTH_ATTACHMENT,
    Stencil = GL_STENCIL_ATTACHMENT,
    DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT
};

class FrameBuffer
{
  public:
    FrameBuffer() { create(); }

    ~FrameBuffer() { release(); }

    // Non-copyable
    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer& operator=(const FrameBuffer&) = delete;

    // Movable
    FrameBuffer(FrameBuffer&& o) noexcept : m_Handle(std::move(o.m_Handle)) {}

    FrameBuffer& operator=(FrameBuffer&& o) noexcept
    {
        if (this != &o)
        {
            release();
            m_Handle = std::move(o.m_Handle);
        }
        return *this;
    }

    void create();

    void bind(GLenum target = GL_FRAMEBUFFER) const { glBindFramebuffer(target, m_Handle.get()); }

    static void unbind(GLenum target = GL_FRAMEBUFFER) { glBindFramebuffer(target, 0); }

    // Attach a texture using OpenGL 4.5+ DSA (Direct State Access)
    void attachTexture(FramebufferAttachment attachment, const TextureHandle& handle,
                       GLint level = 0) const;

    void attachTexture(GLenum attachment, const TextureHandle& handle, GLint level = 0) const;

    // Attach a renderbuffer (e.g. for standalone depth/stencil buffers)
    void attachRenderbuffer(FramebufferAttachment attachment,
                            const RenderBuffer& renderBuffer) const;

    [[nodiscard]] bool isComplete() const;

    void release();

    [[nodiscard]] GLuint id() const { return m_Handle.get(); }
    [[nodiscard]] bool valid() const { return m_Handle.valid(); }

    [[nodiscard]] explicit operator bool() const { return valid(); }

  private:
    FrameBufferHandle m_Handle{};
};

} // namespace sog::gfx