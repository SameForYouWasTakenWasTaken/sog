#pragma once
#include <cmath>
#include <cstdint>

namespace sog::gfx
{
struct Color
{
    static constexpr float MAX_VALUE = 255.f;
    float r, g, b, a = 0;

    explicit constexpr operator math::Vec4() const { return glm::vec4{r, g, b, a}; }

    static constexpr Color fromRGBA(std::uint8_t r, std::uint8_t g, std::uint8_t b,
                                    std::uint8_t a) noexcept
    {
        constexpr float inv255 = 1.0f / 255.0f;
        return {r * inv255, g * inv255, b * inv255, a * inv255};
    }

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