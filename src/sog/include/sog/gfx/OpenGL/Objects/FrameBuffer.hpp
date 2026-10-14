/**
 * @file FrameBuffer.hpp
 * @brief Defines FrameBuffer, a RAII wrapper around a GL framebuffer object for offscreen
 * rendering, plus the FramebufferAttachment enum for slot selection.
 */

#pragma once
#include <cstdint>
#include <sog/gfx/OpenGL/RenderBuffer.hpp>
#include <sog/gfx/Types/Handles.hpp>

#include "glad/gl.h"

namespace sog::gfx
{

/**
 * @brief Framebuffer attachment slots, mirroring the GL_COLOR_ATTACHMENTn / GL_DEPTH_ATTACHMENT /
 * GL_STENCIL_ATTACHMENT / GL_DEPTH_STENCIL_ATTACHMENT constants.
 *
 * Selects where attachTexture()/attachRenderbuffer() plug their resource into the
 * framebuffer.
 */
enum class FramebufferAttachment
{
    Color0 = GL_COLOR_ATTACHMENT0,             /**< First color attachment slot. */
    Color1 = GL_COLOR_ATTACHMENT1,             /**< Second color attachment slot. */
    Color2 = GL_COLOR_ATTACHMENT2,             /**< Third color attachment slot. */
    Color3 = GL_COLOR_ATTACHMENT3,             /**< Fourth color attachment slot. */
    Depth = GL_DEPTH_ATTACHMENT,               /**< Depth-only attachment slot. */
    Stencil = GL_STENCIL_ATTACHMENT,           /**< Stencil-only attachment slot. */
    DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT /**< Combined depth+stencil attachment slot. */
};

/**
 * @brief Owns a GL framebuffer object (FBO) for rendering offscreen.
 *
 * The constructor creates the FBO immediately; attach textures or renderbuffers
 * with the attach* methods, check isComplete() before rendering into it, and bind()
 * it to redirect drawing. Rendered contents can then be sampled as a texture
 * (e.g. for post-processing). Destruction releases the GL object.
 *
 * All attach* methods use GL's direct-state-access entry points, so the FBO does
 * not need to be bound to attach a resource.
 *
 * @attention An FBO is only usable for rendering once it is complete
 * (isComplete() returns true); attaching only a color texture with no depth/
 * stencil where the shader expects one can leave it incomplete.
 *
 * @see sog::FramebufferAttachment, sog::RenderBuffer
 */
class FrameBuffer
{
  public:
    /// Creates the GL framebuffer object (glCreateFramebuffers).
    FrameBuffer() { create(); }

    /// Destroys the owned GL framebuffer.
    ~FrameBuffer() { release(); }

    FrameBuffer(const FrameBuffer&) = delete;            /**< Copy construction is deleted. */
    FrameBuffer& operator=(const FrameBuffer&) = delete; /**< Copy assignment is deleted. */

    /**
     * @brief Move-constructs from another FrameBuffer, taking over its FBO.
     * @param o FrameBuffer to move from; left without an FBO.
     */
    FrameBuffer(FrameBuffer&& o) noexcept : m_Handle(std::move(o.m_Handle)) {}

    /**
     * @brief Move-assigns, releasing the current FBO and taking the source's.
     * @param o FrameBuffer to move from.
     * @return Reference to this FrameBuffer.
     */
    FrameBuffer& operator=(FrameBuffer&& o) noexcept
    {
        if (this != &o)
        {
            release();
            m_Handle = std::move(o.m_Handle);
        }
        return *this;
    }

    /**
     * @brief Creates the GL framebuffer object (no-op if already created).
     */
    void create();

    /**
     * @brief Binds this FBO to the given framebuffer target.
     * @param target Framebuffer target (GL_FRAMEBUFFER by default; GL_READ_FRAMEBUFFER /
     * GL_DRAW_FRAMEBUFFER also valid).
     */
    void bind(GLenum target = GL_FRAMEBUFFER) const { glBindFramebuffer(target, m_Handle.get()); }

    /**
     * @brief Binds the default framebuffer (0) to the given target.
     * @param target Framebuffer target to unbind (GL_FRAMEBUFFER by default).
     */
    static void unbind(GLenum target = GL_FRAMEBUFFER) { glBindFramebuffer(target, 0); }

    /**
     * @brief Attaches a texture to a named attachment slot (DSA, no bind needed).
     * @param attachment Slot to attach to (FramebufferAttachment enumerator).
     * @param handle Texture to attach.
     * @param level Mipmap level to attach (0 for the base level).
     */
    // Attach a texture using OpenGL 4.5+ DSA (Direct State Access)
    void attachTexture(FramebufferAttachment attachment, const TextureHandle& handle,
                       GLint level = 0) const;

    /**
     * @brief Attaches a texture to a raw-GL attachment point (DSA, no bind needed).
     * @param attachment Raw GL attachment constant (e.g. GL_COLOR_ATTACHMENT0).
     * @param handle Texture to attach.
     * @param level Mipmap level to attach (0 for the base level).
     */
    void attachTexture(GLenum attachment, const TextureHandle& handle, GLint level = 0) const;

    /**
     * @brief Attaches a renderbuffer to an attachment slot (DSA, no bind needed).
     * @param attachment Slot to attach to (e.g. FramebufferAttachment::DepthStencil).
     * @param renderBuffer Renderbuffer to attach (commonly for depth/stencil).
     */
    // Attach a renderbuffer (e.g. for standalone depth/stencil buffers)
    void attachRenderbuffer(FramebufferAttachment attachment,
                            const RenderBuffer& renderBuffer) const;

    /**
     * @brief Reports whether the framebuffer is complete and renderable.
     * @return true if glCheckNamedFramebufferStatus reports GL_FRAMEBUFFER_COMPLETE.
     */
    [[nodiscard]] bool isComplete() const;

    /**
     * @brief Deletes the owned FBO (if any) and resets the handle.
     */
    void release();

    /**
     * @brief Returns the raw GL framebuffer name.
     * @return GLuint handle of the FBO.
     */
    [[nodiscard]] GLuint id() const { return m_Handle.get(); }

    /**
     * @brief Reports whether this object currently owns an FBO.
     * @return true if the handle is valid (non-zero).
     */
    [[nodiscard]] bool valid() const { return m_Handle.valid(); }

    /// Contextually converts to bool via valid(); true when an FBO is owned.
    [[nodiscard]] explicit operator bool() const { return valid(); }

  private:
    FrameBufferHandle m_Handle{}; /**< Typed handle to the owned GL framebuffer. */
};

} // namespace sog::gfx
