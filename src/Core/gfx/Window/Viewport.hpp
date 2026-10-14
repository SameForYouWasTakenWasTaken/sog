/**
 * @file Viewport.hpp
 * @brief Defines application::Viewport, a rectangle describing the pixel region of the window that is rendered into.
 */

#pragma once

namespace sog::application
{
/**
 * @brief Position and size (in pixels) of the rendering viewport within the window.
 *
 * Corresponds to the arguments of glViewport(x, y, width, height), where (x, y)
 * is the lower-left corner of the region in window coordinates.
 */
struct Viewport
{
    int x = 0;      /**< Left edge of the viewport in pixels (origin at bottom-left). */
    int y = 0;      /**< Bottom edge of the viewport in pixels. */
    int width = 0;  /**< Viewport width in pixels. */
    int height = 0; /**< Viewport height in pixels. */

    /**
     * @brief Constructs a viewport rectangle.
     * @param x Left edge in pixels.
     * @param y Bottom edge in pixels.
     * @param width Width in pixels.
     * @param height Height in pixels.
     */
    constexpr Viewport(int x, int y, int width, int height)
        : x(x), y(y), width(width), height(height)
    {
    }
};
} // namespace sog::application
