/**
 * @file Input.hpp
 * @brief Declares the sog::input keyboard/mouse query API, including per-frame edge detection,
 * cursor tracking and a camera controller helper.
 *
 * Split into two styles: immediate queries (isKeyDown, isMouseButtonDown,
 * getCursorPosition) that read live GLFW state and need no update(), and
 * edge-triggered queries (isKeyPressed/Released, getCursorDelta) that diff the
 * snapshot taken by update() against the previous frame's.
 */

#pragma once
#include <GLFW/glfw3.h>

#include <sog/gfx/Camera.hpp>
#include <sog/gfx/Window/Window.hpp>

namespace sog::input
{

/**
 * @brief Named keyboard key codes, mirroring GLFW's GLFW_KEY_* constants.
 *
 * Each value is the raw GLFW key code for that key. Grouped as letters, numbers,
 * F-keys, modifiers, punctuation, the numpad and the two international "world"
 * keys. The keyboard queries (isKeyDown, isKeyPressed, isKeyReleased) take a Key
 * directly.
 */
enum class Key
{
    // Letters
    A = GLFW_KEY_A, /**< A key. */
    B = GLFW_KEY_B, /**< B key. */
    C = GLFW_KEY_C, /**< C key. */
    D = GLFW_KEY_D, /**< D key. */
    E = GLFW_KEY_E, /**< E key. */
    F = GLFW_KEY_F, /**< F key. */
    G = GLFW_KEY_G, /**< G key. */
    H = GLFW_KEY_H, /**< H key. */
    I = GLFW_KEY_I, /**< I key. */
    J = GLFW_KEY_J, /**< J key. */
    K = GLFW_KEY_K, /**< K key. */
    L = GLFW_KEY_L, /**< L key. */
    M = GLFW_KEY_M, /**< M key. */
    N = GLFW_KEY_N, /**< N key. */
    O = GLFW_KEY_O, /**< O key. */
    P = GLFW_KEY_P, /**< P key. */
    Q = GLFW_KEY_Q, /**< Q key. */
    R = GLFW_KEY_R, /**< R key. */
    S = GLFW_KEY_S, /**< S key. */
    T = GLFW_KEY_T, /**< T key. */
    U = GLFW_KEY_U, /**< U key. */
    V = GLFW_KEY_V, /**< V key. */
    W = GLFW_KEY_W, /**< W key. */
    X = GLFW_KEY_X, /**< X key. */
    Y = GLFW_KEY_Y, /**< Y key. */
    Z = GLFW_KEY_Z, /**< Z key. */

    // Numbers
    NUM_0 = GLFW_KEY_0, /**< Main-row 0 key. */
    NUM_1 = GLFW_KEY_1, /**< Main-row 1 key. */
    NUM_2 = GLFW_KEY_2, /**< Main-row 2 key. */
    NUM_3 = GLFW_KEY_3, /**< Main-row 3 key. */
    NUM_4 = GLFW_KEY_4, /**< Main-row 4 key. */
    NUM_5 = GLFW_KEY_5, /**< Main-row 5 key. */
    NUM_6 = GLFW_KEY_6, /**< Main-row 6 key. */
    NUM_7 = GLFW_KEY_7, /**< Main-row 7 key. */
    NUM_8 = GLFW_KEY_8, /**< Main-row 8 key. */
    NUM_9 = GLFW_KEY_9, /**< Main-row 9 key. */

    // Function keys
    F1 = GLFW_KEY_F1,   /**< F1 key. */
    F2 = GLFW_KEY_F2,   /**< F2 key. */
    F3 = GLFW_KEY_F3,   /**< F3 key. */
    F4 = GLFW_KEY_F4,   /**< F4 key. */
    F5 = GLFW_KEY_F5,   /**< F5 key. */
    F6 = GLFW_KEY_F6,   /**< F6 key. */
    F7 = GLFW_KEY_F7,   /**< F7 key. */
    F8 = GLFW_KEY_F8,   /**< F8 key. */
    F9 = GLFW_KEY_F9,   /**< F9 key. */
    F10 = GLFW_KEY_F10, /**< F10 key. */
    F11 = GLFW_KEY_F11, /**< F11 key. */
    F12 = GLFW_KEY_F12, /**< F12 key. */
    F13 = GLFW_KEY_F13, /**< F13 key. */
    F14 = GLFW_KEY_F14, /**< F14 key. */
    F15 = GLFW_KEY_F15, /**< F15 key. */
    F16 = GLFW_KEY_F16, /**< F16 key. */
    F17 = GLFW_KEY_F17, /**< F17 key. */
    F18 = GLFW_KEY_F18, /**< F18 key. */
    F19 = GLFW_KEY_F19, /**< F19 key. */
    F20 = GLFW_KEY_F20, /**< F20 key. */
    F21 = GLFW_KEY_F21, /**< F21 key. */
    F22 = GLFW_KEY_F22, /**< F22 key. */
    F23 = GLFW_KEY_F23, /**< F23 key. */
    F24 = GLFW_KEY_F24, /**< F24 key. */
    F25 = GLFW_KEY_F25, /**< F25 key. */

    // Modifiers & Controls
    ESC = GLFW_KEY_ESCAPE,             /**< Escape key. */
    TAB = GLFW_KEY_TAB,                /**< Tab key. */
    ENTER = GLFW_KEY_ENTER,            /**< Enter key. */
    SPACE = GLFW_KEY_SPACE,            /**< Space bar. */
    BACKSPACE = GLFW_KEY_BACKSPACE,    /**< Backspace key. */
    INSERT = GLFW_KEY_INSERT,          /**< Insert key. */
    DELETE = GLFW_KEY_DELETE,          /**< Delete key. */
    RIGHT = GLFW_KEY_RIGHT,            /**< Right arrow key. */
    LEFT = GLFW_KEY_LEFT,              /**< Left arrow key. */
    DOWN = GLFW_KEY_DOWN,              /**< Down arrow key. */
    UP = GLFW_KEY_UP,                  /**< Up arrow key. */
    PAGE_UP = GLFW_KEY_PAGE_UP,        /**< Page up key. */
    PAGE_DOWN = GLFW_KEY_PAGE_DOWN,    /**< Page down key. */
    HOME = GLFW_KEY_HOME,              /**< Home key. */
    END = GLFW_KEY_END,                /**< End key. */
    CAPS_LOCK = GLFW_KEY_CAPS_LOCK,    /**< Caps lock key. */
    SCROLL_LOCK = GLFW_KEY_SCROLL_LOCK, /**< Scroll lock key. */
    NUM_LOCK = GLFW_KEY_NUM_LOCK,      /**< Num lock key. */
    PRINT_SCREEN = GLFW_KEY_PRINT_SCREEN, /**< Print screen key. */
    PAUSE = GLFW_KEY_PAUSE,            /**< Pause key. */

    // Special characters / Punctuation
    APOSTROPHE = GLFW_KEY_APOSTROPHE,   /**< Apostrophe key ('). */
    COMMA = GLFW_KEY_COMMA,             /**< Comma key (,). */
    MINUS = GLFW_KEY_MINUS,             /**< Minus key (-). */
    PERIOD = GLFW_KEY_PERIOD,           /**< Period key (.). */
    SLASH = GLFW_KEY_SLASH,             /**< Slash key (/). */
    SEMICOLON = GLFW_KEY_SEMICOLON,     /**< Semicolon key (;). */
    EQUAL = GLFW_KEY_EQUAL,             /**< Equal key (=). */
    LEFT_BRACKET = GLFW_KEY_LEFT_BRACKET,   /**< Left bracket key. */
    BACKSLASH = GLFW_KEY_BACKSLASH,         /**< Backslash key. */
    RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET, /**< Right bracket key. */
    GRAVE_ACCENT = GLFW_KEY_GRAVE_ACCENT,   /**< Grave accent key. */

    // Modifiers (Left / Right variants)
    LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT,     /**< Left shift key. */
    LEFT_CTRL = GLFW_KEY_LEFT_CONTROL,    /**< Left control key. */
    LEFT_ALT = GLFW_KEY_LEFT_ALT,         /**< Left alt key. */
    LEFT_SUPER = GLFW_KEY_LEFT_SUPER,     /**< Left "super" key (Windows/Command). */
    RIGHT_SHIFT = GLFW_KEY_RIGHT_SHIFT,   /**< Right shift key. */
    RIGHT_CTRL = GLFW_KEY_RIGHT_CONTROL,  /**< Right control key. */
    RIGHT_ALT = GLFW_KEY_RIGHT_ALT,       /**< Right alt key. */
    RIGHT_SUPER = GLFW_KEY_RIGHT_SUPER,   /**< Right "super" key. */
    MENU = GLFW_KEY_MENU,                 /**< Menu key. */

    // Numpad
    KP_0 = GLFW_KEY_KP_0,             /**< Numpad 0 key. */
    KP_1 = GLFW_KEY_KP_1,             /**< Numpad 1 key. */
    KP_2 = GLFW_KEY_KP_2,             /**< Numpad 2 key. */
    KP_3 = GLFW_KEY_KP_3,             /**< Numpad 3 key. */
    KP_4 = GLFW_KEY_KP_4,             /**< Numpad 4 key. */
    KP_5 = GLFW_KEY_KP_5,             /**< Numpad 5 key. */
    KP_6 = GLFW_KEY_KP_6,             /**< Numpad 6 key. */
    KP_7 = GLFW_KEY_KP_7,             /**< Numpad 7 key. */
    KP_8 = GLFW_KEY_KP_8,             /**< Numpad 8 key. */
    KP_9 = GLFW_KEY_KP_9,             /**< Numpad 9 key. */
    KP_DECIMAL = GLFW_KEY_KP_DECIMAL, /**< Numpad decimal key. */
    KP_DIVIDE = GLFW_KEY_KP_DIVIDE,   /**< Numpad divide key. */
    KP_MULTIPLY = GLFW_KEY_KP_MULTIPLY, /**< Numpad multiply key. */
    KP_SUBTRACT = GLFW_KEY_KP_SUBTRACT, /**< Numpad subtract key. */
    KP_ADD = GLFW_KEY_KP_ADD,         /**< Numpad add key. */
    KP_ENTER = GLFW_KEY_KP_ENTER,     /**< Numpad enter key. */
    KP_EQUAL = GLFW_KEY_KP_EQUAL,     /**< Numpad equal key. */

    // General terms
    WORLD_1 = GLFW_KEY_WORLD_1, /**< International key 1. */
    WORLD_2 = GLFW_KEY_WORLD_2  /**< International key 2. */
};

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
 * @param key Key to test.
 * @return true if the key is down right now.
 */
[[nodiscard]] bool isKeyDown(const gfx::Window& window, Key key);

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
 * @param key Key to test.
 * @return true if the key is down now but was up at the previous update().
 */
[[nodiscard]] bool isKeyPressed(Key key); // went down this frame

/**
 * @brief Reports whether a key went up this frame (requires update()).
 * @param key Key to test.
 * @return true if the key is up now but was down at the previous update().
 */
[[nodiscard]] bool isKeyReleased(Key key); // went up this frame

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
 * @param captured true to disable/hide the cursor and lock it to the window; false to restore
 * normal mode.
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
