/**
 * @file Input.cpp
 * @brief Implements the sog::input API: GLFW-backed live queries, per-frame edge detection via a module-local state snapshot, cursor tracking and the camera controller.
 */

#include "Input.hpp"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <array>

#include "glad/gl.h"

namespace sog::input
{
namespace
{
/**
 * @brief Fetches the raw GLFWwindow* from an engine Window.
 * @param w Engine window whose native handle is wanted.
 * @return Underlying GLFWwindow pointer.
 */
GLFWwindow* native(const gfx::Window& w) { return static_cast<GLFWwindow*>(w.getNativeHandle()); }

/**
 * @brief Module-local snapshot of keyboard/mouse/cursor state for edge detection.
 *
 * Holds the current and previous frame's key and button states (so Pressed/
 * Released can be computed by diffing), plus the last cursor position and the
 * per-frame movement delta.
 */
struct State
{
    std::array<bool, GLFW_KEY_LAST + 1> keys{}, prevKeys{};       /**< Current then previous frame's key-down states. */
    std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> buttons{}, prevButtons{}; /**< Current then previous frame's button-down states. */
    glm::vec2 cursor{}, delta{}; /**< Last cursor position and movement since the prior update(). */
    bool hasCursor{false};       /**< Whether a prior cursor position exists (gates the first delta). */
};

State g_State; /**< Global input state shared by all queries in this translation unit. */

/**
 * @brief Bounds-checks a key/button code against a state array size.
 * @param v Code to test.
 * @param size Number of elements in the corresponding state array.
 * @return true if v is within [0, size).
 */
bool inRange(int v, std::size_t size) { return v >= 0 && static_cast<std::size_t>(v) < size; }
} // namespace

void update(const gfx::Window& window)
{
    GLFWwindow* w = native(window);

    g_State.prevKeys = g_State.keys;
    g_State.prevButtons = g_State.buttons;

    for (int k = GLFW_KEY_SPACE; k <= GLFW_KEY_LAST; ++k) // keys below SPACE are invalid
        g_State.keys[k] = glfwGetKey(w, k) == GLFW_PRESS;

    for (int b = 0; b <= GLFW_MOUSE_BUTTON_LAST; ++b)
        g_State.buttons[b] = glfwGetMouseButton(w, b) == GLFW_PRESS;

    const glm::vec2 now = getCursorPosition(window);
    // No delta on the first frame, otherwise the camera jumps
    g_State.delta = g_State.hasCursor ? now - g_State.cursor : glm::vec2{0.f};
    g_State.cursor = now;
    g_State.hasCursor = true;
}

bool isKeyDown(const gfx::Window& window, int key)
{
    return glfwGetKey(native(window), key) == GLFW_PRESS;
}

bool isMouseButtonDown(const gfx::Window& window, int button)
{
    return glfwGetMouseButton(native(window), button) == GLFW_PRESS;
}

glm::vec2 getCursorPosition(const gfx::Window& window)
{
    double x{}, y{};
    glfwGetCursorPos(native(window), &x, &y);
    return {static_cast<float>(x), static_cast<float>(y)};
}

bool isKeyPressed(int key)
{
    return inRange(key, g_State.keys.size()) && g_State.keys[key] && !g_State.prevKeys[key];
}

bool isKeyReleased(int key)
{
    return inRange(key, g_State.keys.size()) && !g_State.keys[key] && g_State.prevKeys[key];
}

bool isMouseButtonPressed(int button)
{
    return inRange(button, g_State.buttons.size()) && g_State.buttons[button] &&
           !g_State.prevButtons[button];
}

bool isMouseButtonReleased(int button)
{
    return inRange(button, g_State.buttons.size()) && !g_State.buttons[button] &&
           g_State.prevButtons[button];
}

glm::vec2 getCursorDelta() { return g_State.delta; }

void setCursorCaptured(const gfx::Window& window, bool captured)
{
    glfwSetInputMode(native(window), GLFW_CURSOR,
                     captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    g_State.hasCursor = false; // avoid a delta spike when the mode switches
}
void updateCamera(gfx::Camera& camera, const gfx::Window& window, float dt, float moveSpeed,
                  float turnSpeed)
{
    // Look: arrow keys
    float yaw = 0.f, pitch = 0.f;
    if (isKeyDown(window, GLFW_KEY_LEFT))
        yaw -= 1.f;
    if (isKeyDown(window, GLFW_KEY_RIGHT))
        yaw += 1.f;
    if (isKeyDown(window, GLFW_KEY_UP))
        pitch += 1.f;
    if (isKeyDown(window, GLFW_KEY_DOWN))
        pitch -= 1.f;

    if (yaw != 0.f || pitch != 0.f)
    {
        const float newPitch = std::clamp(camera.getPitch() + pitch * turnSpeed * dt, -89.f, 89.f);
        camera.setRotation(camera.getYaw() + yaw * turnSpeed * dt, newPitch);
    }

    // Move: WASD + Space/Shift, in camera space
    glm::vec3 dir{0.f}; // x = right, y = up, z = forward
    if (isKeyDown(window, GLFW_KEY_W))
        dir.z += 1.f;
    if (isKeyDown(window, GLFW_KEY_S))
        dir.z -= 1.f;
    if (isKeyDown(window, GLFW_KEY_D))
        dir.x += 1.f;
    if (isKeyDown(window, GLFW_KEY_A))
        dir.x -= 1.f;
    if (isKeyDown(window, GLFW_KEY_SPACE))
        dir.y += 1.f;
    if (isKeyDown(window, GLFW_KEY_LEFT_CONTROL))
        dir.y -= 1.f;

    if (glm::dot(dir, dir) > 0.f)
    {
        dir = glm::normalize(dir) * moveSpeed * dt; // same speed on diagonals
        camera.moveLocal(dir.x, dir.y, dir.z);
    }
}
} // namespace sog::input