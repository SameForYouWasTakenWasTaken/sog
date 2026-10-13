#pragma once
#include <cassert>
#include <cstdint>

#include "gfx/GLHandle.hpp"
#include "glad/gl.h"
namespace sog::gfx::backend
{

template <typename T> class GLBuffer
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
            glNamedBufferSubData(id_.get(), byteOffset, bytes, data);
    }

    // Storage is immutable in GL 4.5, so "resizing" = new buffer + GPU-side copy
    void resize(size_t newBytes, bool keepContents)
    {
        GLBuffer next(newBytes, flags_);
        if (keepContents && id_.get())
            glCopyNamedBufferSubData(id_.get(), next.id_.get(), 0, 0, std::min(size_, newBytes));
        swap(next); // 'next' now holds the old buffer and frees it
    }

    void bindBuffer() const { glBindBuffer(GL_ARRAY_BUFFER, id_.get()); }
    void bindBinding(GLenum target, GLuint binding) const
    {
        glBindBufferBase(target, binding, id_.get());
    }
    void bindRange(GLenum target, GLuint binding, size_t off, size_t bytes) const
    {
        glBindBufferRange(target, binding, id_.get(), off, bytes);
    }

    [[nodiscard]] GLuint id() const { return id_.get(); }
    [[nodiscard]] size_t size() const { return size_; }

  private:
    void create(size_t bytes, GLbitfield flags)
    {
        flags_ = flags;
        size_ = bytes;
        glCreateBuffers(1, &id_.raw());
        glNamedBufferStorage(id_.get(), bytes, nullptr, flags);
    }
    void destroy()
    {
        if (id_.valid())
            glDeleteBuffers(1, &id_.raw());
        id_.raw() = 0;
        size_ = 0;
    }
    void swap(GLBuffer& o) noexcept
    {
        std::swap(id_.raw(), o.id_.raw());
        std::swap(size_, o.size_);
        std::swap(flags_, o.flags_);
    }

    GLHandle<T> id_{};
    size_t size_ = 0;
    GLbitfield flags_ = GL_DYNAMIC_STORAGE_BIT;
};
} // namespace sog::gfx::backend