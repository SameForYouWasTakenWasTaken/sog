/**
 * @file Engine.cpp
 * @brief Implements engine::Engine: constructor delegates to the (currently empty) initialize() hook.
 */

#include "Engine.hpp"

#include <glad/gl.h>

namespace sog::engine
{

/**
 * @brief Constructs the Engine and runs one-time initialization.
 */
Engine::Engine() { initialize(); }

/**
 * @brief One-time setup hook; currently empty (no initialization performed).
 */
void Engine::initialize() {}
} // namespace sog::engine
