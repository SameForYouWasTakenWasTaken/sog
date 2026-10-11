#pragma once
#include <cassert>
#include <cstdint>

#include "glad/gl.h"

namespace sog::gfx::backend
{
class GLBuffer
{
  public:
    GLBuffer() = default;
    GLBuffer(size_t bytes, GLbitfield flags = GL_DYNAMIC_STORAGE_BIT) { create(bytes, flags); }
    ~GLBuffer() { destroy(); }

    GLBuffer(const GLBuffer&) = delete;
    GLBuffer& operator=(const GLBuffer&) = delete;
    GLBuffer(GLBuffer&& o) noexcept { swap(o); }
    GLBuffer& operator=(GLBuffer&& o) noexcept
    {
        if (this != &o)
        {
            destroy();
            swap(o);
        }
        return *this;
    }

    void write(size_t byteOffset, const void* data, size_t bytes)
    {
        assert(byteOffset + bytes <= size_);
        if (bytes)
            glNamedBufferSubData(id_, byteOffset, bytes, data);
    }

    // Storage is immutable in GL 4.5, so "resizing" = new buffer + GPU-side copy
    void resize(size_t newBytes, bool keepContents)
    {
        GLBuffer next(newBytes, flags_);
        if (keepContents && id_)
            glCopyNamedBufferSubData(id_, next.id_, 0, 0, std::min(size_, newBytes));
        swap(next); // 'next' now holds the old buffer and frees it
    }

    void bind(GLenum target, GLuint binding) const { glBindBufferBase(target, binding, id_); }
    void bindRange(GLenum target, GLuint binding, size_t off, size_t bytes) const
    {
        glBindBufferRange(target, binding, id_, off, bytes);
    }

    GLuint id() const { return id_; }
    size_t size() const { return size_; }

  private:
    void create(size_t bytes, GLbitfield flags)
    {
        flags_ = flags;
        size_ = bytes;
        glCreateBuffers(1, &id_);
        glNamedBufferStorage(id_, bytes, nullptr, flags);
    }
    void destroy()
    {
        if (id_)
            glDeleteBuffers(1, &id_);
        id_ = 0;
        size_ = 0;
    }
    void swap(GLBuffer& o) noexcept
    {
        std::swap(id_, o.id_);
        std::swap(size_, o.size_);
        std::swap(flags_, o.flags_);
    }

    GLuint id_ = 0;
    size_t size_ = 0;
    GLbitfield flags_ = GL_DYNAMIC_STORAGE_BIT;
};
} // namespace sog::gfx::backend