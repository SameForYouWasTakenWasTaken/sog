#pragma once
#include "gfx/Camera.hpp"
#include "gfx/Window/Window.hpp"

namespace sog::input
{
// Call once per frame, right after Window::pollEvents().
// Required for the *Pressed / *Released / delta functions.
void update(const gfx::Window& window);

// Immediate state (no update() needed)
[[nodiscard]] bool isKeyDown(const gfx::Window& window, int key);
[[nodiscard]] bool isMouseButtonDown(const gfx::Window& window, int button);
[[nodiscard]] glm::vec2 getCursorPosition(const gfx::Window& window);

// Edge-triggered (state from update())
[[nodiscard]] bool isKeyPressed(int key);  // went down this frame
[[nodiscard]] bool isKeyReleased(int key); // went up this frame
[[nodiscard]] bool isMouseButtonPressed(int button);
[[nodiscard]] bool isMouseButtonReleased(int button);
[[nodiscard]] glm::vec2 getCursorDelta(); // movement since last update()

void setCursorCaptured(const gfx::Window& window, bool captured); // FPS-style mouse look

// WASD + Space/Shift to move, arrow keys to look
void updateCamera(gfx::Camera& camera, const gfx::Window& window, float dt, float moveSpeed = 5.0f,
                  float turnSpeed = 90.0f);
} // namespace sog::input