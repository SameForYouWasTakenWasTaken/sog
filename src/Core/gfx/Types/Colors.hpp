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

    float r{}, g{}, b{}, a = 0; /**< Color channels; r/g/b default to 0.0 (black), a defaults to 0.0
                                   (fully transparent). */

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
     * @brief Scales float channels into the 0-255 range, rounding each to a whole number.
     * @param r Red channel, expected in [0, 1].
     * @param g Green channel, expected in [0, 1].
     * @param b Blue channel, expected in [0, 1].
     * @param a Alpha channel, expected in [0, 1].
     * @return Color whose members hold the rounded 0-255 values as floats.
     *
     * @attention Despite the name, this does not produce 8-bit integers; it returns a
     * Color whose float members are scaled to 0-255. The result is only suitable as
     * an intermediate for further conversion (e.g. before writing bytes to disk),
     * not for direct use with OpenGL calls that expect normalized [0, 1] colors.
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
