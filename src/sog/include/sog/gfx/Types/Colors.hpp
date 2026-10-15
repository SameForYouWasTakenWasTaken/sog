/**
 * @file Colors.hpp
 * @brief Defines sog::gfx::Color, a float RGBA color with helpers for converting to and from
 * 8-bit-per-channel values.
 */

#pragma once
#include <array>
#include <cstdint>

#include "../../Utility/Math.hpp"

namespace sog::gfx
{
/**
 * @brief RGBA color stored as normalized floats (0.0 - 1.0 per channel).
 *
 * Matches the component range expected by OpenGL (e.g. glClearColor, glClear),
 * so a Color can be handed straight to the API. Use fromRGBA() to build a color
 * from conventional 8-bit values, and toRGBA() to get 8-bit channels back out.
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
     * @brief Converts this color's normalized channels into conventional 8-bit values
     * (the inverse of fromRGBA()).
     * @return Array of four bytes {r, g, b, a}, each channel scaled by 255 and rounded to the
     * nearest integer (halfway values round up). Values outside [0, 1] are clamped first.
     *
     * @code
     * auto red = Color{1.0f, 0.0f, 0.0f, 1.0f}.toRGBA(); // red == 255, alpha == 255
     * @endcode
     */
    [[nodiscard]] constexpr std::array<std::uint8_t, 4> toRGBA() const noexcept
    {
        const auto toByte = [](float channel) -> std::uint8_t
        {
            if (channel <= 0.f)
                return 0;
            if (channel >= 1.f)
                return static_cast<std::uint8_t>(MAX_VALUE);
            // Rounded via +0.5f truncation instead of std::round, which is not constexpr before
            // C++23 and would make this constexpr function ill-formed under C++20.
            return static_cast<std::uint8_t>(channel * MAX_VALUE + 0.5f);
        };
        return {toByte(r), toByte(g), toByte(b), toByte(a)};
    }
};
} // namespace sog::gfx
