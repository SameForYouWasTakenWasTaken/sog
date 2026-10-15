/**
 * @file OpenGLDebug.cpp
 * @brief Implements the sog::debug OpenGL debug-output bridge: GL-enum-to-string translation and
 * the glDebugMessageCallback installer.
 */

#include <sog/gfx/backend/OpenGLDebug.hpp>

namespace sog::debug
{
/**
 * @brief Converts a GL debug source enum into a readable string.
 * @param source GL_DEBUG_SOURCE_* constant.
 * @return Static string such as "API", "WINDOW_SYSTEM", "SHADER_COMPILER", "THIRD_PARTY",
 * "APPLICATION", "OTHER", or "UNKNOWN".
 */
const char* debugSourceToString(GLenum source)
{
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        return "API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        return "WINDOW_SYSTEM";
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        return "SHADER_COMPILER";
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        return "THIRD_PARTY";
    case GL_DEBUG_SOURCE_APPLICATION:
        return "APPLICATION";
    case GL_DEBUG_SOURCE_OTHER:
        return "OTHER";
    default:
        return "UNKNOWN";
    }
}

/**
 * @brief Converts a GL debug type enum into a readable string.
 * @param type GL_DEBUG_TYPE_* constant.
 * @return Static string such as "ERROR", "DEPRECATED_BEHAVIOR", "PERFORMANCE", "MARKER", or
 * "UNKNOWN".
 */
const char* debugTypeToString(GLenum type)
{
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        return "ERROR";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        return "DEPRECATED_BEHAVIOR";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        return "UNDEFINED_BEHAVIOR";
    case GL_DEBUG_TYPE_PORTABILITY:
        return "PORTABILITY";
    case GL_DEBUG_TYPE_PERFORMANCE:
        return "PERFORMANCE";
    case GL_DEBUG_TYPE_MARKER:
        return "MARKER";
    case GL_DEBUG_TYPE_PUSH_GROUP:
        return "PUSH_GROUP";
    case GL_DEBUG_TYPE_POP_GROUP:
        return "POP_GROUP";
    case GL_DEBUG_TYPE_OTHER:
        return "OTHER";
    default:
        return "UNKNOWN";
    }
}

/**
 * @brief Converts a GL debug severity enum into a readable string.
 * @param severity GL_DEBUG_SEVERITY_* constant.
 * @return Static string such as "HIGH", "MEDIUM", "LOW", "NOTIFICATION", or "UNKNOWN".
 */
const char* debugSeverityToString(GLenum severity)
{
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        return "HIGH";
    case GL_DEBUG_SEVERITY_MEDIUM:
        return "MEDIUM";
    case GL_DEBUG_SEVERITY_LOW:
        return "LOW";
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        return "NOTIFICATION";
    default:
        return "UNKNOWN";
    }
}

/// The most recently registered user callback (replaced by setDebugCallback).
static DebugCallback s_UserCallback = nullptr;

/**
 * @brief Internal bridge installed with glDebugMessageCallback; translates enums and forwards to
 * the user callback.
 *
 * The GL length argument is unused (the message is NUL-terminated), as is
 * userParam: the bridge reads the module-level s_UserCallback instead.
 *
 * @param source GL debug source enum.
 * @param type GL debug type enum.
 * @param id GL debug message id.
 * @param severity GL debug severity enum.
 * @param message The debug message text.
 */
void GLAD_API_PTR internalOpenGlBridge(GLenum source, GLenum type, GLuint id, GLenum severity,
                                       GLsizei /*length*/, const GLchar* message,
                                       const void* /*userParam*/)
{
    // If the user registered a callback, translate the GL enums to strings and call it
    if (s_UserCallback)
    {
        s_UserCallback(debugSourceToString(source), debugTypeToString(type),
                       debugSeverityToString(severity), id, message);
    }
}

// 2. This is the clean function your user calls
/**
 * @brief Stores the user callback and enables OpenGL debug output.
 *
 * Installs internalOpenGlBridge as the GL debug callback, then enables
 * GL_DEBUG_OUTPUT and GL_DEBUG_OUTPUT_SYNCHRONOUS.
 */
void setDebugCallback(DebugCallback callback)
{
    s_UserCallback = std::move(callback);

    glDebugMessageCallback(internalOpenGlBridge, nullptr);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}

} // namespace sog::debug