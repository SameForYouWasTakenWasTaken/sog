/**
 * @file FrameBuffer.cpp
 * @brief Implements FrameBuffer: FBO creation, DSA-based texture/renderbuffer attachment,
 * completeness check and release.
 */

#include <sog/Error/Exceptions/graphics_error.hpp>
#include <sog/gfx/OpenGL/Objects/FrameBuffer.hpp>

#include "glad/gl.h"
namespace sog::gfx
{
void FrameBuffer::create()
{
    if (!m_Handle.valid())
    {
        glCreateFramebuffers(1, &m_Handle.raw());
    }
}
void FrameBuffer::attachTexture(FramebufferAttachment attachment, const TextureHandle& handle,
                                GLint level) const
{
    glNamedFramebufferTexture(m_Handle.get(), static_cast<GLenum>(attachment), handle.get(), level);
}
void FrameBuffer::attachTexture(GLenum attachment, const TextureHandle& handle, GLint level) const
{
    glNamedFramebufferTexture(m_Handle.get(), attachment, handle.get(), level);
}
void FrameBuffer::attachRenderbuffer(FramebufferAttachment attachment,
                                     const RenderBuffer& renderBuffer) const
{
    glNamedFramebufferRenderbuffer(m_Handle.get(), static_cast<GLenum>(attachment), GL_RENDERBUFFER,
                                   renderBuffer.id());
}
bool FrameBuffer::isComplete() const
{
    return glCheckNamedFramebufferStatus(m_Handle.get(), GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}
void FrameBuffer::release()
{
    if (m_Handle.valid())
    {
        glDeleteFramebuffers(1, &m_Handle.raw());
        m_Handle.raw() = 0;
    }
}
} // namespace sog::gfx