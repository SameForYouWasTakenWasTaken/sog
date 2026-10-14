/**
 * @file Event.hpp
 * @brief Defines EventType and Event, the engine's window/event payload (currently close and resize).
 */

#pragma once

namespace sog::gfx
{

/**
 * @brief Discriminator telling which payload an Event carries.
 */
enum class EventType
{
    Closed, /**< The window was requested to close (e.g. user clicked the close button). */
    Resized /**< The window/framebuffer was resized; payload in Event::size. */
};

/**
 * @brief A window event: a type tag plus a union payload describing the event.
 *
 * Read type first, then inspect the matching union member (currently only size,
 * valid for EventType::Resized).
 *
 * @note The payload is a union so an Event stays small regardless of how many
 * event kinds are added later. The nested anonymous struct inside the union is a
 * widely supported compiler extension (MSVC, GCC, Clang) rather than strict
 * standard C++, but it behaves as an ordinary struct member here.
 */
struct Event
{
    EventType type; /**< Which event this is; determines the valid union member. */
    union
    {
        struct
        {
            int width;  /**< New width in pixels (EventType::Resized). */
            int height; /**< New height in pixels (EventType::Resized). */
        } size;         /**< Payload for EventType::Resized events. */
    };
};

} // namespace sog::gfx
