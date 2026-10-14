/**
 * @file Input.hpp
 * @brief Declares the sog::input keyboard/mouse query API, including per-frame edge detection, cursor tracking and a camera controller helper.
 *
 * Split into two styles: immediate queries (isKeyDown, isMouseButtonDown,
 * getCursorPosition) that read live GLFW state and need no update(), and
 * edge-triggered queries (isKeyPressed/Released, getCursorDelta) that diff the
 * snapshot taken by update() against the previous frame's.
 */

#pragma once
#include "gfx/Camera.hpp"
#include "gfx/Window/Window.hpp"

namespace sog::input
{
// Call once per frame, right after Window::pollEvents().
// Required for the *Pressed / *Released / delta functions.
/**
 * @brief Snapshots the current key/mouse/cursor state for edge-triggered queries.
 * @param window Whose native input is sampled.
 *
 * Call once per frame, immediately after Window::pollEvents(). Advances the
 * previous-state buffers, polls every key and mouse button, and computes the
 * cursor delta since the prior update (zero on the very first update, to avoid a
 * spurious jump).
 */
void update(const gfx::Window& window);

// Immediate state (no update() needed)
/**
 * @brief Reports whether a key is currently held (live query, no update() needed).
 * @param window Whose native input is queried.
 * @param key GLFW key code to test.
 * @return true if the key is down right now.
 */
[[nodiscard]] bool isKeyDown(const gfx::Window& window, int key);

/**
 * @brief Reports whether a mouse button is currently held (live query, no update() needed).
 * @param window Whose native input is queried.
 * @param button GLFW mouse-button code to test.
 * @return true if the button is down right now.
 */
[[nodiscard]] bool isMouseButtonDown(const gfx::Window& window, int button);

/**
 * @brief Returns the current cursor position in window coordinates.
 * @param window Whose native input is queried.
 * @return Cursor position (x right, y down) in pixels, as floats.
 */
[[nodiscard]] glm::vec2 getCursorPosition(const gfx::Window& window);

// Edge-triggered (state from update())
/**
 * @brief Reports whether a key went down this frame (requires update()).
 * @param key GLFW key code to test.
 * @return true if the key is down now but was up at the previous update().
 */
[[nodiscard]] bool isKeyPressed(int key);  // went down this frame

/**
 * @brief Reports whether a key went up this frame (requires update()).
 * @param key GLFW key code to test.
 * @return true if the key is up now but was down at the previous update().
 */
[[nodiscard]] bool isKeyReleased(int key); // went up this frame

/**
 * @brief Reports whether a mouse button went down this frame (requires update()).
 * @param button GLFW mouse-button code to test.
 * @return true if the button is down now but was up at the previous update().
 */
[[nodiscard]] bool isMouseButtonPressed(int button);

/**
 * @brief Reports whether a mouse button went up this frame (requires update()).
 * @param button GLFW mouse-button code to test.
 * @return true if the button is up now but was down at the previous update().
 */
[[nodiscard]] bool isMouseButtonReleased(int button);

/**
 * @brief Returns how far the cursor moved since the last update() (requires update()).
 * @return Change in cursor position over the last frame.
 */
[[nodiscard]] glm::vec2 getCursorDelta(); // movement since last update()

/**
 * @brief Captures (or releases) the cursor for FPS-style mouse look.
 * @param window Whose native input mode is changed.
 * @param captured true to disable/hide the cursor and lock it to the window; false to restore normal mode.
 *
 * Resets the stored cursor position so the mode switch does not produce a huge
 * delta spike on the next update().
 */
void setCursorCaptured(const gfx::Window& window, bool captured); // FPS-style mouse look

// WASD + Space/Shift to move, arrow keys to look
/**
 * @brief Drives a Camera from the keyboard: WASD to move, Space/Ctrl for up/down, arrows to look.
 * @param camera Camera to manipulate in place.
 * @param window Whose input is read (live queries).
 * @param dt Frame delta time in seconds; scales movement and turning for frame-rate independence.
 * @param moveSpeed Units per second of horizontal/vertical movement (default 5).
 * @param turnSpeed Degrees per second of arrow-key turning (default 90).
 *
 * Movement is normalized so diagonal travel is not faster, and pitch is clamped
 * to +/-89 degrees to avoid flipping. Call after update() if you also need
 * edge-triggered queries the same frame; the movement/look reads use live state.
 */
void updateCamera(gfx::Camera& camera, const gfx::Window& window, float dt, float moveSpeed = 5.0f,
                  float turnSpeed = 90.0f);
} // namespace sog::input
