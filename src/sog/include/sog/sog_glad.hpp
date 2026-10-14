/**
 * @file sog_glad.hpp
 * @brief Convenience header exposing the vendored glad OpenGL loader to library consumers.
 *
 * Includes glad/gl.h from src/sog/vendor (the library's public include path),
 * so a program that needs GL entry points of its own can include
 * <sog/sog_glad.hpp> instead of reaching into the vendor directory. The library
 * itself loads the function pointers at runtime via gladLoadGL in Window's
 * initialization.
 */

#pragma once
#include "glad/gl.h"

namespace sog
{
} // namespace sog
