/**
 * @file Uniform.hpp
 * @brief Defines Uniform, a small value type wrapping a shader uniform's GLint location with validity checks.
 */

#pragma once
#include "glad/gl.h"
namespace sog::gfx
{
/**
 * @brief Location of a uniform within a linked shader program.
 *
 * Wraps the GLint returned by glGetUniformLocation. A location of -1 means the
 * uniform does not exist in the program (or is unused/optimized out); valid(),
 * operator bool and the explicit conversion to Location let callers check before
 * setting values.
 *
 * @see sog::ShaderProgram::getLocation, sog::uniform::getLocation
 */
struct Uniform
{
    using Location = GLint; /**< Underlying GL location type (GLint). */

    Location location = -1; /**< Raw location; -1 means "not found"/invalid. */

    /**
     * @brief Reports whether this location names an existing uniform.
     * @return true if the location is not -1.
     */
    [[nodiscard]] constexpr bool valid() const noexcept { return location != -1; }

    /// Contextually converts to bool via valid(); true when the uniform exists.
    [[nodiscard]] constexpr explicit operator bool() const noexcept { return valid(); }

    /**
     * @brief Explicitly converts to the raw GL location.
     * @return The stored GLint location (possibly -1).
     */
    [[nodiscard]] constexpr explicit operator Location() const noexcept { return location; }
};
} // namespace sog::gfx
