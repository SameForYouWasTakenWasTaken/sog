/**
 * @file OpenGLDebug.hpp
 * @brief Declares the sog::debug OpenGL debug-output API: a DebugCallback type and setDebugCallback() for receiving translated GL debug messages.
 */

#pragma once

#include <functional>
#include <glad/gl.h>

namespace sog::debug
{
// Internal helper strings (hidden in an anonymous namespace to avoid global clutter)

/**
 * @brief Signature of a user handler for translated OpenGL debug messages.
 *
 * The GL enums are already converted to human-readable strings by the internal
 * bridge before this is invoked: source and type are e.g. "API" / "ERROR",
 * severity e.g. "HIGH" / "NOTIFICATION".
 *
 * @param source Human-readable debug source (e.g. "API", "SHADER_COMPILER").
 * @param type Human-readable debug type (e.g. "ERROR", "PERFORMANCE").
 * @param severity Human-readable severity (e.g. "HIGH", "MEDIUM", "LOW", "NOTIFICATION").
 * @param id The raw GL debug message id.
 * @param message The debug message text.
 */
using DebugCallback = std::function<void(const char* source, const char* type, const char* severity,
                                         GLuint id, const char* message)>;

/**
 * @brief Registers a handler for OpenGL debug messages and enables debug output.
 *
 * Stores the callback, installs an internal bridge with glDebugMessageCallback and
 * enables GL_DEBUG_OUTPUT (plus synchronous output for reliable debuggers). Every
 * subsequent GL debug message is translated and forwarded to callback.
 *
 * @param callback Handler to receive translated debug messages; replaces any previous one.
 *
 * @note Requires a debug context (the engine requests one in Window::initialize via
 * GLFW_OPENGL_DEBUG_CONTEXT).
 */
void setDebugCallback(DebugCallback callback);
} // namespace sog::debug
