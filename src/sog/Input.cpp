/**
 * @file Input.cpp
 * @brief Implements the sog::input API: GLFW-backed live queries, per-frame edge detection via a
 * module-local state snapshot, cursor tracking and the camera controller.
 */

#include "include/sog/Input.hpp"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <array>

#include "glad/gl.h"

namespace sog::input
{
enum class Key
{
    // Letters
    A = GLFW_KEY_A,
    B = GLFW_KEY_B,
    C = GLFW_KEY_C,
    D = GLFW_KEY_D,
    E = GLFW_KEY_E,
    F = GLFW_KEY_F,
    G = GLFW_KEY_G,
    H = GLFW_KEY_H,
    I = GLFW_KEY_I,
    J = GLFW_KEY_J,
    K = GLFW_KEY_K,
    L = GLFW_KEY_L,
    M = GLFW_KEY_M,
    N = GLFW_KEY_N,
    O = GLFW_KEY_O,
    P = GLFW_KEY_P,
    Q = GLFW_KEY_Q,
    R = GLFW_KEY_R,
    S = GLFW_KEY_S,
    T = GLFW_KEY_T,
    U = GLFW_KEY_U,
    V = GLFW_KEY_V,
    W = GLFW_KEY_W,
    X = GLFW_KEY_X,
    Y = GLFW_KEY_Y,
    Z = GLFW_KEY_Z,

    // Numbers
    NUM_0 = GLFW_KEY_0,
    NUM_1 = GLFW_KEY_1,
    NUM_2 = GLFW_KEY_2,
    NUM_3 = GLFW_KEY_3,
    NUM_4 = GLFW_KEY_4,
    NUM_5 = GLFW_KEY_5,
    NUM_6 = GLFW_KEY_6,
    NUM_7 = GLFW_KEY_7,
    NUM_8 = GLFW_KEY_8,
    NUM_9 = GLFW_KEY_9,

    // Function keys
    F1 = GLFW_KEY_F1,
    F2 = GLFW_KEY_F2,
    F3 = GLFW_KEY_F3,
    F4 = GLFW_KEY_F4,
    F5 = GLFW_KEY_F5,
    F6 = GLFW_KEY_F6,
    F7 = GLFW_KEY_F7,
    F8 = GLFW_KEY_F8,
    F9 = GLFW_KEY_F9,
    F10 = GLFW_KEY_F10,
    F11 = GLFW_KEY_F11,
    F12 = GLFW_KEY_F12,
    F13 = GLFW_KEY_F13,
    F14 = GLFW_KEY_F14,
    F15 = GLFW_KEY_F15,
    F16 = GLFW_KEY_F16,
    F17 = GLFW_KEY_F17,
    F18 = GLFW_KEY_F18,
    F19 = GLFW_KEY_F19,
    F20 = GLFW_KEY_F20,
    F21 = GLFW_KEY_F21,
    F22 = GLFW_KEY_F22,
    F23 = GLFW_KEY_F23,
    F24 = GLFW_KEY_F24,
    F25 = GLFW_KEY_F25,

    // Modifiers & Controls
    ESC = GLFW_KEY_ESCAPE,
    TAB = GLFW_KEY_TAB,
    ENTER = GLFW_KEY_ENTER,
    SPACE = GLFW_KEY_SPACE,
    BACKSPACE = GLFW_KEY_BACKSPACE,
    INSERT = GLFW_KEY_INSERT,
    DELETE = GLFW_KEY_DELETE,
    RIGHT = GLFW_KEY_RIGHT,
    LEFT = GLFW_KEY_LEFT,
    DOWN = GLFW_KEY_DOWN,
    UP = GLFW_KEY_UP,
    PAGE_UP = GLFW_KEY_PAGE_UP,
    PAGE_DOWN = GLFW_KEY_PAGE_DOWN,
    HOME = GLFW_KEY_HOME,
    END = GLFW_KEY_END,
    CAPS_LOCK = GLFW_KEY_CAPS_LOCK,
    SCROLL_LOCK = GLFW_KEY_SCROLL_LOCK,
    NUM_LOCK = GLFW_KEY_NUM_LOCK,
    PRINT_SCREEN = GLFW_KEY_PRINT_SCREEN,
    PAUSE = GLFW_KEY_PAUSE,

    // Special characters / Punctuation
    APOSTROPHE = GLFW_KEY_APOSTROPHE,
    COMMA = GLFW_KEY_COMMA,
    MINUS = GLFW_KEY_MINUS,
    PERIOD = GLFW_KEY_PERIOD,
    SLASH = GLFW_KEY_SLASH,
    SEMICOLON = GLFW_KEY_SEMICOLON,
    EQUAL = GLFW_KEY_EQUAL,
    LEFT_BRACKET = GLFW_KEY_LEFT_BRACKET,
    BACKSLASH = GLFW_KEY_BACKSLASH,
    RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET,
    GRAVE_ACCENT = GLFW_KEY_GRAVE_ACCENT,

    // Modifiers (Left / Right variants)
    LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT,
    LEFT_CTRL = GLFW_KEY_LEFT_CONTROL,
    LEFT_ALT = GLFW_KEY_LEFT_ALT,
    LEFT_SUPER = GLFW_KEY_LEFT_SUPER,
    RIGHT_SHIFT = GLFW_KEY_RIGHT_SHIFT,
    RIGHT_CTRL = GLFW_KEY_RIGHT_CONTROL,
    RIGHT_ALT = GLFW_KEY_RIGHT_ALT,
    RIGHT_SUPER = GLFW_KEY_RIGHT_SUPER,
    MENU = GLFW_KEY_MENU,

    // Numpad
    KP_0 = GLFW_KEY_KP_0,
    KP_1 = GLFW_KEY_KP_1,
    KP_2 = GLFW_KEY_KP_2,
    KP_3 = GLFW_KEY_KP_3,
    KP_4 = GLFW_KEY_KP_4,
    KP_5 = GLFW_KEY_KP_5,
    KP_6 = GLFW_KEY_KP_6,
    KP_7 = GLFW_KEY_KP_7,
    KP_8 = GLFW_KEY_KP_8,
    KP_9 = GLFW_KEY_KP_9,
    KP_DECIMAL = GLFW_KEY_KP_DECIMAL,
    KP_DIVIDE = GLFW_KEY_KP_DIVIDE,
    KP_MULTIPLY = GLFW_KEY_KP_MULTIPLY,
    KP_SUBTRACT = GLFW_KEY_KP_SUBTRACT,
    KP_ADD = GLFW_KEY_KP_ADD,
    KP_ENTER = GLFW_KEY_KP_ENTER,
    KP_EQUAL = GLFW_KEY_KP_EQUAL,

    // General terms
    WORLD_1 = GLFW_KEY_WORLD_1,
    WORLD_2 = GLFW_KEY_WORLD_2
};
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
    std::array<bool, GLFW_KEY_LAST + 1> keys{},
        prevKeys{}; /**< Current then previous frame's key-down states. */
    std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> buttons{},
        prevButtons{};           /**< Current then previous frame's button-down states. */
    glm::vec2 cursor{}, delta{}; /**< Last cursor position and movement since the prior update(). */
    bool hasCursor{false}; /**< Whether a prior cursor position exists (gates the first delta). */
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