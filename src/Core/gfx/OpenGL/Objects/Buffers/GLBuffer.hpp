/**
 * @file GLBuffer.hpp
 * @brief Defines backend::GLBuffer<T>, a move-only RAII wrapper around a GL 4.5 immutable-storage buffer object (glCreateBuffers/glNamedBufferStorage).
 */

#pragma once
#include <cassert>
#include <cstdint>

#include "gfx/GLHandle.hpp"
#include "glad/gl.h"
namespace sog::gfx::backend
{

/**
 * @brief Owns one OpenGL buffer object with immutable storage (DSA-style, GL 4.5+).
 *
 * Creates the buffer with glCreateBuffers + glNamedBufferStorage, deleting it on
 * destruction. Storage is immutable per the GL spec, so resize() implements
 * "growing" by allocating a fresh buffer and GPU-copying the old contents into it.
 * Copying is disabled; moving transfers the handle.
 *
 * The class is used through the higher-level sog::GpuBuffer, which tracks element
 * counts and capacity on top of this byte-oriented buffer.
 *
 * @tparam T Element type of the data stored (used for the typed handle only; this
 * buffer is byte-addressed via write()).
 *
 * @attention write() asserts that byteOffset + bytes does not exceed the current
 * size; the check is compiled out in release builds.
 *
 * @see sog::GpuBuffer
 */
template <typename T> class GLBuffer
{
  public:
    /// Constructs an empty buffer with no GL object allocated.
    GLBuffer() = default;

    /**
     * @brief Creates a buffer with immutable storage of the requested byte size.
     * @param bytes Storage size in bytes.
     * @param flags Immutable-storage flags (default GL_DYNAMIC_STORAGE_BIT allows glNamedBufferSubData writes).
     */
    GLBuffer(size_t bytes, GLbitfield flags = GL_DYNAMIC_STORAGE_BIT) { create(bytes, flags); }

    /// Destroys the underlying GL buffer object (if any).
    ~GLBuffer() { destroy(); }

    GLBuffer(const GLBuffer&) = delete;            /**< Copy construction is deleted. */
    GLBuffer& operator=(const GLBuffer&) = delete; /**< Copy assignment is deleted. */

    /**
     * @brief Move-constructs by swapping GL identity with the source.
     * @param o Buffer to take ownership from; left empty.
     */
    GLBuffer(GLBuffer&& o) noexcept { swap(o); }

    /**
     * @brief Move-assigns, destroying the current buffer and taking the source's.
     * @param o Buffer to take ownership from; left empty.
     * @return Reference to this buffer.
     */
    GLBuffer& operator=(GLBuffer&& o) noexcept
    {
        if (this != &o)
        {
            destroy();
            swap(o);
        }
        return *this;
    }

    /**
     * @brief Writes bytes into the buffer at a byte offset (glNamedBufferSubData).
     * @param byteOffset Destination offset in bytes from the start of the buffer.
     * @param data Source bytes to copy in.
     * @param bytes Number of bytes to write; a zero length is a no-op.
     */
    void write(size_t byteOffset, const void* data, size_t bytes)
    {
        assert(byteOffset + bytes <= size_);
        if (bytes)
            glNamedBufferSubData(id_.get(), byteOffset, bytes, data);
    }

    // Storage is immutable in GL 4.5, so "resizing" = new buffer + GPU-side copy
    /**
     * @brief Reallocates the buffer to a new byte size, optionally copying old contents.
     *
     * Because GL 4.5 storage is immutable, this creates a new buffer of newBytes
     * (with the original creation flags), copies min(old size, newBytes) bytes
     * across on the GPU when keepContents is true and a source exists, then swaps
     * the new buffer in and destroys the old one.
     *
     * @param newBytes New storage size in bytes.
     * @param keepContents Whether to copy the old contents into the new buffer.
     */
    void resize(size_t newBytes, bool keepContents)
    {
        GLBuffer next(newBytes, flags_);
        if (keepContents && id_.get())
            glCopyNamedBufferSubData(id_.get(), next.id_.get(), 0, 0, std::min(size_, newBytes));
        swap(next); // 'next' now holds the old buffer and frees it
    }

    /// Binds this buffer to the GL_ARRAY_BUFFER target of the current context.
    void bindBuffer() const { glBindBuffer(GL_ARRAY_BUFFER, id_.get()); }

    /**
     * @brief Binds the whole buffer to an indexed binding point (glBindBufferBase).
     * @param target Buffer target (e.g. GL_UNIFORM_BUFFER, GL_SHADER_STORAGE_BUFFER).
     * @param binding Index of the binding point.
     */
    void bindBinding(GLenum target, GLuint binding) const
    {
        glBindBufferBase(target, binding, id_.get());
    }

    /**
     * @brief Binds a byte range of this buffer to an indexed binding point (glBindBufferRange).
     * @param target Buffer target (e.g. GL_UNIFORM_BUFFER).
     * @param binding Index of the binding point.
     * @param off Byte offset of the range start.
     * @param bytes Byte length of the range.
     */
    void bindRange(GLenum target, GLuint binding, size_t off, size_t bytes) const
    {
        glBindBufferRange(target, binding, id_.get(), off, bytes);
    }

    /**
     * @brief Returns the raw GL buffer name.
     * @return GLuint handle (0 if no buffer was created).
     */
    [[nodiscard]] GLuint id() const { return id_.get(); }

    /**
     * @brief Returns the current storage size in bytes.
     * @return Size passed at creation or by the latest resize().
     */
    [[nodiscard]] size_t size() const { return size_; }

  private:
    /**
     * @brief Creates the GL buffer and allocates its immutable storage.
     * @param bytes Storage size in bytes.
     * @param flags Immutable-storage creation flags.
     */
    void create(size_t bytes, GLbitfield flags)
    {
        flags_ = flags;
        size_ = bytes;
        glCreateBuffers(1, &id_.raw());
        glNamedBufferStorage(id_.get(), bytes, nullptr, flags);
    }

    /// Deletes the GL buffer (if any) and resets the handle and size.
    void destroy()
    {
        if (id_.valid())
            glDeleteBuffers(1, &id_.raw());
        id_.raw() = 0;
        size_ = 0;
    }

    /**
     * @brief Swaps GL identity, size and flags with another buffer.
     * @param o Buffer to swap with.
     */
    void swap(GLBuffer& o) noexcept
    {
        std::swap(id_.raw(), o.id_.raw());
        std::swap(size_, o.size_);
        std::swap(flags_, o.flags_);
    }

    GLHandle<T> id_{};                 /**< Typed GL object name for this buffer. */
    size_t size_ = 0;                  /**< Current storage size in bytes. */
    GLbitfield flags_ = GL_DYNAMIC_STORAGE_BIT; /**< Creation flags, remembered for resize(). */
};
} // namespace sog::gfx::backend
