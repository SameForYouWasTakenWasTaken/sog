/**
 * @file RenderBuffer.hpp
 * @brief Defines RenderBuffer, a RAII wrapper around a GL renderbuffer object (fixed-size,
 * format-specified offscreen storage, commonly for depth/stencil).
 */

#pragma once
#include <cstdint>
#include <sog/gfx/Types/Handles.hpp>
#include <utility>

#include "glad/gl.h"

namespace sog::gfx
{

/**
 * @brief Owns a GL renderbuffer object for offscreen storage of a fixed format and size.
 *
 * Unlike a texture, a renderbuffer's contents cannot be sampled directly; it is
 * meant to be attached to a framebuffer (e.g. as depth/stencil storage). Storage
 * is allocated at construction or via create() using GL's direct-state-access
 * glNamedRenderbufferStorage, so it need not be bound.
 *
 * The object is move-only; destruction or release() frees the GL renderbuffer and
 * resets the recorded dimensions and format.
 *
 * @see sog::FrameBuffer::attachRenderbuffer
 */
class RenderBuffer
{
  public:
    /// Constructs an empty renderbuffer with no GL object allocated.
    RenderBuffer() = default;

    /**
     * @brief Creates a renderbuffer and allocates its storage immediately.
     * @param internalFormat GL internal format (e.g. GL_DEPTH24_STENCIL8).
     * @param width Width in pixels.
     * @param height Height in pixels.
     */
    RenderBuffer(GLenum internalFormat, GLsizei width, GLsizei height)
    {
        create(internalFormat, width, height);
    }

    /// Destroys the owned GL renderbuffer (if any).
    ~RenderBuffer() { release(); }

    RenderBuffer(const RenderBuffer&) = delete;            /**< Copy construction is deleted. */
    RenderBuffer& operator=(const RenderBuffer&) = delete; /**< Copy assignment is deleted. */

    /**
     * @brief Move-constructs from another RenderBuffer, taking over its GL object.
     * @param o RenderBuffer to move from; left empty (0 dimensions/format).
     */
    RenderBuffer(RenderBuffer&& o) noexcept
        : m_Handle(std::move(o.m_Handle)), m_Width(std::exchange(o.m_Width, 0)),
          m_Height(std::exchange(o.m_Height, 0)),
          m_InternalFormat(std::exchange(o.m_InternalFormat, 0))
    {
    }

    /**
     * @brief Move-assigns, releasing the current renderbuffer and taking the source's.
     * @param o RenderBuffer to move from.
     * @return Reference to this RenderBuffer.
     */
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

    /**
     * @brief Creates the GL renderbuffer (if needed) and allocates its storage.
     * @param internalFormat GL internal format of the storage.
     * @param width Width in pixels.
     * @param height Height in pixels.
     */
    void create(GLenum internalFormat, GLsizei width, GLsizei height);

    /**
     * @brief Binds this renderbuffer to the given target.
     * @param target Renderbuffer target (GL_RENDERBUFFER by default).
     */
    void bind(GLenum target = GL_RENDERBUFFER) const { glBindRenderbuffer(target, m_Handle.get()); }

    /**
     * @brief Binds the default renderbuffer (0) to the given target.
     * @param target Target to unbind (GL_RENDERBUFFER by default).
     */
    static void unbind(GLenum target = GL_RENDERBUFFER) { glBindRenderbuffer(target, 0); }

    /**
     * @brief Deletes the owned renderbuffer (if any) and resets dimensions/format.
     */
    void release();

    /**
     * @brief Returns the raw GL renderbuffer name.
     * @return GLuint handle of the renderbuffer.
     */
    [[nodiscard]] GLuint id() const { return m_Handle.get(); }

    /**
     * @brief Returns the allocated width in pixels.
     * @return Width passed to create() (0 if never created or released).
     */
    [[nodiscard]] GLsizei width() const { return m_Width; }

    /**
     * @brief Returns the allocated height in pixels.
     * @return Height passed to create() (0 if never created or released).
     */
    [[nodiscard]] GLsizei height() const { return m_Height; }

    /**
     * @brief Returns the GL internal format of the storage.
     * @return Internal format passed to create() (0 if never created or released).
     */
    [[nodiscard]] GLenum internalFormat() const { return m_InternalFormat; }

    /**
     * @brief Reports whether this object currently owns a renderbuffer.
     * @return true if the handle is valid (non-zero).
     */
    [[nodiscard]] bool valid() const { return m_Handle.valid(); }

    /// Contextually converts to bool via valid(); true when a renderbuffer is owned.
    [[nodiscard]] explicit operator bool() const { return valid(); }

  private:
    RenderBufferHandle m_Handle{}; /**< Typed handle to the owned GL renderbuffer. */
    GLsizei m_Width{0};            /**< Allocated width in pixels. */
    GLsizei m_Height{0};           /**< Allocated height in pixels. */
    GLenum m_InternalFormat{0};    /**< GL internal format of the allocated storage. */
};

} // namespace sog::gfx
