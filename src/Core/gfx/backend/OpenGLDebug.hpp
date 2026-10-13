#pragma once

#include <functional>
#include <glad/gl.h>

namespace sog::debug
{
// Internal helper strings (hidden in an anonymous namespace to avoid global clutter)

using DebugCallback = std::function<void(const char* source, const char* type, const char* severity,
                                         GLuint id, const char* message)>;

void setDebugCallback(DebugCallback callback);
} // namespace sog::debug