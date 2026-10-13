#pragma once
#include <utility>

#include "Types/GFXTypes.hpp"
#include "glad/gl.h"

namespace sog::gfx
{

// ReSharper disable once CppTemplateParameterNeverUsed
template <typename Tag> class GLHandle
{
  public:
    using value_type = gl_handle;
    using tag_type = Tag;

    GLHandle() = default;
    virtual ~GLHandle() = default;

    GLHandle(const GLHandle&) = delete;
    GLHandle& operator=(const GLHandle&) = delete;

    GLHandle(GLHandle&& other) noexcept : m_Handle(std::exchange(other.m_Handle, 0)) {}

    GLHandle& operator=(GLHandle&& other) noexcept
    {
        if (this != &other)
            m_Handle = std::exchange(other.m_Handle, 0);
        return *this;
    }

    [[nodiscard]] constexpr GLuint get() const noexcept { return m_Handle; }
    GLuint& raw() noexcept { return m_Handle; }

    [[nodiscard]] constexpr bool valid() const noexcept { return m_Handle != 0; }

    constexpr explicit operator bool() const noexcept { return valid(); }

  protected:
    gl_handle m_Handle{};
};
} // namespace sog::gfx