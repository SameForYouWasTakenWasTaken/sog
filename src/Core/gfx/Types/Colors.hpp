#pragma once
#include <cmath>
#include <cstdint>

namespace sog::gfx
{
struct Color
{
    static constexpr float MAX_VALUE = 255.f;
    float r{}, g{}, b{}, a = 0;

    explicit constexpr operator math::Vec4() const { return glm::vec4{r, g, b, a}; }

    static constexpr Color fromRGBA(std::uint8_t r, std::uint8_t g, std::uint8_t b,
                                    std::uint8_t a) noexcept
    {
        constexpr float inv255 = 1.0f / 255.0f;
        return {static_cast<float>(r) * inv255, static_cast<float>(g) * inv255,
                static_cast<float>(b) * inv255, static_cast<float>(a) * inv255};
    }

    static constexpr Color toRGBA(float r, float g, float b, float a) noexcept
    {
        constexpr float max = 255.0f;
        return {std::round(r * max),

                std::round(g * max),

                std::round(b * max),

                std::round(a * max)};
    }
};
} // namespace sog::gfx