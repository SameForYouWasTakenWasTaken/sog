/**
 * @file Time.hpp
 * @brief Declares sog::Time, a tiny utility exposing the current high-resolution time.
 */

#pragma once

namespace sog
{
/**
 * @brief Static access to the current time (seconds since GLFW initialization).
 *
 * Wraps glfwGetTime(), so a GLFW context must have been initialized (e.g. by
 * creating a Window) before calling. Useful for frame timing and delta-time
 * computation.
 */
struct Time
{
    /**
     * @brief Returns the current time in seconds.
     * @return Elapsed seconds (as a double) since GLFW was initialized.
     */
    static double getTime();
};
} // namespace sog
