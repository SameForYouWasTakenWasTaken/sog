#pragma once

namespace sog::application
{
struct Viewport
{
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;

    constexpr Viewport(int x, int y, int width, int height)
        : x(x), y(y), width(width), height(height)
    {
    }
};
} // namespace sog::application