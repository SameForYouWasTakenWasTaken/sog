/**
 * @file Colors.hpp
 * @brief Defines sog::gfx::Color, an RGBA color on the conventional 8-bit-per-channel scale
 * (0 - 255), with helpers for converting to the normalized values OpenGL expects.
 */

#pragma once
#include <array>
#include <cstdint>

#include "../../Utility/Math.hpp"

namespace sog::gfx
{
/**
 * @brief RGBA color on the conventional 8-bit-per-channel scale (0 - 255 per channel).
 *
 * Matches how people naturally write colors ({255, 128, 0} orange, like a color
 * picker or CSS), so a Color literal reads the way it looks. Internals hand the
 * value to OpenGL through normalized() (or the explicit Vec4 conversion), which
 * divides each channel by MAX_VALUE into the [0, 1] range glClearColor and
 * friends expect — callers never divide by hand.
 *
 * fromRGBA() constructs from uint8_t-typed sources (e.g. image data), and
 * toRGBA() converts back to bytes (e.g. for serialization).
 */
struct Color
{
    static constexpr float MAX_VALUE =
        255.f; /**< Largest channel value (8-bit scale); divisor for normalized(). */

    float r{};           /**< Red channel (0 - 255); defaults to 0 (black). */
    float g{};           /**< Green channel (0 - 255); defaults to 0 (black). */
    float b{};           /**< Blue channel (0 - 255); defaults to 0 (black). */
    float a = MAX_VALUE; /**< Alpha channel (0 - 255); defaults to 255 (opaque) so an RGB-only
                              literal such as Color{255, 0, 0} stays fully visible. */

    /**
     * @brief Converts this color to the normalized form OpenGL expects (GL calls, shaders).
     * @return glm::vec4 holding r, g, b, a each divided by MAX_VALUE, i.e. in [0, 1].
     *
     * @code
     * auto n = Color{255, 128, 0, 255}.normalized(); // n.x == 1.0f, n.y ~= 0.502f
     * @endcode
     */
    [[nodiscard]] constexpr math::Vec4 normalized() const
    {
        return {r / MAX_VALUE, g / MAX_VALUE, b / MAX_VALUE, a / MAX_VALUE};
    }

    /**
     * @brief Converts this color to a normalized 4-component vector for GL calls.
     * @return glm::vec4 with the channels in r, g, b, a order, each scaled into [0, 1]
     * (identical to normalized(); kept as a conversion operator for explicit casts).
     */
    explicit constexpr operator math::Vec4() const { return normalized(); }

    /**
     * @brief Builds a Color from conventional 8-bit-per-channel values.
     * @param r Red channel, 0-255.
     * @param g Green channel, 0-255.
     * @param b Blue channel, 0-255.
     * @param a Alpha channel, 0-255.
     * @return Color whose channels hold the given 8-bit values on the 0-255 scale.
     *
     * @code
     * auto red = Color::fromRGBA(255, 0, 0, 255); // r == 255.0f, a == 255.0f
     * @endcode
     */
    static constexpr Color fromRGBA(std::uint8_t r, std::uint8_t g, std::uint8_t b,
                                    std::uint8_t a) noexcept
    {
        return {static_cast<float>(r), static_cast<float>(g), static_cast<float>(b),
                static_cast<float>(a)};
    }

    /**
     * @brief Converts this color's channels into conventional 8-bit values (the inverse of
     * fromRGBA()).
     * @return Array of four bytes {r, g, b, a}, each channel rounded to the nearest integer
     * (halfway values round up). Values outside [0, 255] are clamped first.
     *
     * @code
     * auto red = Color{255.0f, 0.0f, 0.0f, 255.0f}.toRGBA(); // red == 255, alpha == 255
     * @endcode
     */
    [[nodiscard]] constexpr std::array<std::uint8_t, 4> toRGBA() const noexcept
    {
        const auto toByte = [](float channel) -> std::uint8_t
        {
            if (channel <= 0.f)
                return 0;
            if (channel >= MAX_VALUE)
                return static_cast<std::uint8_t>(MAX_VALUE);
            // Rounded via +0.5f truncation instead of std::round, which is not constexpr before
            // C++23 and would make this constexpr function ill-formed under C++20.
            return static_cast<std::uint8_t>(channel + 0.5f);
        };
        return {toByte(r), toByte(g), toByte(b), toByte(a)};
    }
};
} // namespace sog::gfx
