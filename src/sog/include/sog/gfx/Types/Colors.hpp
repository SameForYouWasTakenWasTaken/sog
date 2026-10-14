/**
 * @file Colors.hpp
 * @brief Defines sog::gfx::Color, a float RGBA color with helpers for converting to and from
 * 8-bit-per-channel values.
 */

#pragma once
#include <cmath>
#include <cstdint>

namespace sog::gfx
{
/**
 * @brief RGBA color stored as normalized floats (0.0 - 1.0 per channel).
 *
 * Matches the component range expected by OpenGL (e.g. glClearColor, glClear),
 * so a Color can be handed straight to the API. Use fromRGBA() to build a color
 * from conventional 8-bit values.
 *
 * @attention The members are assumed to be normalized floats in [0, 1]. See
 * toRGBA() for the (unexpected) behaviour of that helper before using it.
 */
struct Color
{
    static constexpr float MAX_VALUE =
        255.f; /**< Largest value of an 8-bit channel; scale factor for toRGBA(). */

    float r{}; /**< Red channel (0.0 - 1.0); defaults to 0.0 (black). */
    float g{}; /**< Green channel (0.0 - 1.0); defaults to 0.0 (black). */
    float b{}; /**< Blue channel (0.0 - 1.0); defaults to 0.0 (black). */
    float a = 0; /**< Alpha channel (0.0 - 1.0); defaults to 0.0 (fully transparent). */

    /**
     * @brief Converts this color to a 4-component vector for GL calls.
     * @return glm::vec4 holding the r, g, b, a components in that order.
     */
    explicit constexpr operator math::Vec4() const { return glm::vec4{r, g, b, a}; }

    /**
     * @brief Builds a normalized Color from conventional 8-bit-per-channel values.
     * @param r Red channel, 0-255.
     * @param g Green channel, 0-255.
     * @param b Blue channel, 0-255.
     * @param a Alpha channel, 0-255.
     * @return Color with each channel divided by 255 into the [0, 1] range.
     *
     * @code
     * auto red = Color::fromRGBA(255, 0, 0, 255); // r == 1.0f, a == 1.0f
     * @endcode
     */
    static constexpr Color fromRGBA(std::uint8_t r, std::uint8_t g, std::uint8_t b,
                                    std::uint8_t a) noexcept
    {
        constexpr float inv255 = 1.0f / 255.0f;
        return {static_cast<float>(r) * inv255, static_cast<float>(g) * inv255,
                static_cast<float>(b) * inv255, static_cast<float>(a) * inv255};
    }

    /**
     * @brief Scales 8-bit channels into whole-number 0-255 float values.
     * @param r Red channel, 0-255.
     * @param g Green channel, 0-255.
     * @param b Blue channel, 0-255.
     * @param a Alpha channel, 0-255.
     * @return Color whose members hold the rounded, scaled channel values as floats.
     *
     * @attention Despite the name, this does not produce normalized 8-bit values
     * usable with OpenGL: the parameters are 0-255 inputs, and the current
     * implementation multiplies them by 255 again (0-255 becomes 0-65025). Treat
     * this helper as unusable until it is fixed; use fromRGBA() to build a
     * normalized [0, 1] Color instead.
     */
    static constexpr Color toRGBA(std::uint8_t r, std::uint8_t g, std::uint8_t b,
                                  std::uint8_t a) noexcept
    {
        constexpr float max = 255.0f;
        return {std::round(r * max),

                std::round(g * max),

                std::round(b * max),

                std::round(a * max)};
    }
};
} // namespace sog::gfx
