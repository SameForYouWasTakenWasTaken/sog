/**
 * @file graphics_error.hpp
 * @brief Defines sog::graphics_error, a base exception carrying a machine-readable
 * graphics_error_code for graphics/OpenGL failures.
 */

#pragma once
#include <sog/Error/exception.hpp>
#include <utility>

namespace sog
{
/**
 * @brief Machine-readable codes describing why a graphics operation failed.
 *
 * Each enumerator corresponds to a distinct class of graphics failure (window
 * creation, shader compilation, buffer/texture/framebuffer state, etc.) so that
 * callers can branch on the failure kind via graphics_error::code() without
 * parsing the human-readable message.
 */
enum class graphics_error_code
{
    window_creation_failed,       /**< The window (and its OpenGL context) could not be created. */
    invalid_operation,            /**< An OpenGL operation was attempted in an invalid state. */
    shader_compilation_failed,    /**< A shader stage failed to compile. */
    shader_link_failed,           /**< A shader program failed to link. */
    buffer_creation_failed,       /**< An OpenGL buffer object could not be created. */
    vertex_array_creation_failed, /**< A vertex array object could not be created. */
    vertex_array_not_bound,   /**< An operation required a bound vertex array object, but none was
                                 bound. */
    buffer_not_bound,         /**< An operation required a bound buffer, but none was bound. */
    uniform_location_invalid, /**< A uniform was looked up by a name that does not exist in the
                                 linked program. */

    shader_file_open_failed, /**< A shader source file could not be opened for reading. */
    invalid_shader_program,  /**< An invalid shader program or an invalid OpenGL handle */

    texture_load_failed, /**< Texture data could not be loaded or decoded. */
    texture_not_bound,   /**< An operation required a bound texture, but none was bound. */

    frame_buffer_invalid,   /**< A framebuffer is incomplete or otherwise not usable. */
    frame_buffer_not_bound, /**< An operation required a bound framebuffer, but none was bound. */
};

/**
 * @brief Exception type for graphics/OpenGL failures, carrying a graphics_error_code.
 *
 * Derives from sog::exception, so it also captures the human-readable message and
 * the std::source_location of the throw site. Use code() to branch on the failure
 * kind programmatically.
 *
 * @see sog::exception, sog::graphics_error_code
 */
class graphics_error : public exception
{
  public:
    /**
     * @brief Constructs a graphics_error with a specific failure code and message.
     * @param code Machine-readable classification of the failure.
     * @param message Human-readable description of what went wrong.
     * @param location Capture point of the throw; defaults to the construction site.
     */
    graphics_error(graphics_error_code code, std::string message,
                   std::source_location location = std::source_location::current())
        : exception(std::move(message), location), m_code(code)
    {
    }

    /**
     * @brief Returns the machine-readable code classifying this failure.
     * @return The graphics_error_code this exception was constructed with.
     */
    [[nodiscard]]
    graphics_error_code code() const noexcept
    {
        return m_code;
    }

  private:
    graphics_error_code m_code; /**< Classification of the graphics failure. */
};
} // namespace sog