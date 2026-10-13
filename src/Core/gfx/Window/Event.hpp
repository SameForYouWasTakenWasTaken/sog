#pragma once

namespace sog::gfx
{

enum class EventType
{
    Closed,
    Resized
};

struct Event
{
    EventType type;
    union
    {
        struct
        {
            int width;
            int height;
        } size;
    };
};

} // namespace sog::gfx