/**
 * @file GLHandle.hpp
 * @brief Defines GLHandle<Tag>, a move-only typed wrapper around a raw OpenGL object name.
 *
 * The Tag parameter is a phantom type: it makes handles for different GL object
 * kinds (buffers, textures, shaders, ...) mutually non-convertible even though
 * every handle stores the same underlying GLuint.
 */

#pragma once
#include <utility>

#include "Types/GFXTypes.hpp"
#include "glad/gl.h"

namespace sog::gfx
{

/**
 * @brief Move-only RAII-style name holder for an OpenGL object.
 *
 * Owns nothing beyond the GLuint name — destruction does NOT delete the GL
 * object; it is a typed, non-copyable identity for one. Copying is disabled so a
 * name is never duplicated implicitly; moving transfers the name and zeroes the
 * source, leaving the moved-from handle invalid().
 *
 * @tparam Tag Phantom type distinguishing handle kinds; see sog::gfx handles in
 * Handles.hpp (e.g. VertexArrayHandle = GLHandle<VertexArrayTag>).
 *
 * @attention value 0 is the GL "no object" name; valid() (and operator bool)
 * report false for it, and a default-constructed handle is therefore invalid.
 *
 * @see sog::gfx::Handles.hpp for the concrete handle types
 */
// ReSharper disable once CppTemplateParameterNeverUsed
template <typename Tag> class GLHandle
{
  public:
    /// Underlying raw GL object name type.
    using value_type = gl_handle;
    /// The phantom tag type distinguishing this handle kind.
    using tag_type = Tag;

    /// Constructs an invalid (zero) handle.
    GLHandle() = default;
    /// Virtual destructor; makes GLHandle a polymorphic base if a subclass adds deletion.
    virtual ~GLHandle() = default;

    GLHandle(const GLHandle&) = delete; /**< Copy construction is deleted; handles are unique. */
    GLHandle&
    operator=(const GLHandle&) = delete; /**< Copy assignment is deleted; handles are unique. */

    /**
     * @brief Move-constructs from another handle, taking over its name.
     * @param other Handle to steal from; left invalid (0) afterwards.
     */
    GLHandle(GLHandle&& other) noexcept : m_Handle(std::exchange(other.m_Handle, 0)) {}

    /**
     * @brief Move-assigns from another handle, taking over its name.
     * @param other Handle to steal from; left invalid (0) afterwards.
     * @return Reference to this handle.
     */
    GLHandle& operator=(GLHandle&& other) noexcept
    {
        if (this != &other)
            m_Handle = std::exchange(other.m_Handle, 0);
        return *this;
    }

    /**
     * @brief Returns the raw GL object name.
     * @return The stored GLuint name (0 if invalid).
     */
    [[nodiscard]] constexpr GLuint get() const noexcept { return m_Handle; }

    /**
     * @brief Returns a mutable reference to the stored name.
     * @return Reference to the internal GLuint, for filling in a name returned by glGen*.
     */
    GLuint& raw() noexcept { return m_Handle; }

    /**
     * @brief Reports whether this handle names a real GL object.
     * @return true if the stored name is non-zero.
     */
    [[nodiscard]] constexpr bool valid() const noexcept { return m_Handle != 0; }

    /// Contextually converts to bool via valid(); true when the handle names a real object.
    constexpr explicit operator bool() const noexcept { return valid(); }

    /// Contextually converts to gl_handle (unsigned int) via get().
    operator gl_handle() const noexcept { return get(); }

  protected:
    gl_handle m_Handle{}; /**< The raw GL object name; 0 means "no object". */
};
} // namespace sog::gfx
