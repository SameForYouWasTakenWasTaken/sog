/**
 * @file WindowContext.hpp
 * @brief Defines context::WindowContext, the configuration struct describing the window size, title and requested OpenGL version/profile.
 */

#pragma once
#include <cstdint>
#include <string>

namespace sog::context
{
/**
 * @brief Creation parameters for a Window: OpenGL version and profile, dimensions and title.
 *
 * Passed to the sog::gfx::Window constructor; defaults request a 4.6 core-profile
 * context on an 800x600 window with an empty title.
 */
struct WindowContext
{
    /**
     * @brief Which OpenGL context profile to request from the driver.
     */
    enum class OpenGLProfile : int
    {
        CORE,         /**< Core profile: only modern, deprecation-free entry points. */
        COMPATIBILITY, /**< Compatibility profile: legacy entry points remain available. */
    };

    int OpenGL_Version_Major = 4;  /**< Major version of the OpenGL context to request. */
    int OpenGL_Version_Minor = 6;  /**< Minor version of the OpenGL context to request. */
    OpenGLProfile OpenGL_Profile = OpenGLProfile::CORE; /**< Requested context profile. */

    int width = 800;  /**< Initial window width in pixels. */
    int height = 600; /**< Initial window height in pixels. */
    std::string title{}; /**< Window title shown in the title bar. */
};
} // namespace sog::context
