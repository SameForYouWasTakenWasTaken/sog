/**
 * @file Engine.hpp
 * @brief Declares engine::Engine, a currently minimal top-level engine object whose constructor runs initialization.
 */

#pragma once

namespace sog::engine
{
/**
 * @brief Top-level engine facade; currently a skeleton that runs initialize() on construction.
 *
 * Constructing an Engine performs one-time setup via the private initialize()
 * hook, which is presently empty. This is the intended home for global engine
 * initialization (subsystems, logging, etc.) as the project grows.
 */
class Engine
{
  public:
    /**
     * @brief Constructs the engine and runs one-time initialization.
     */
    Engine();

  private:
    /**
     * @brief One-time setup hook invoked by the constructor (currently a no-op).
     */
    void initialize();
};
} // namespace sog::engine
