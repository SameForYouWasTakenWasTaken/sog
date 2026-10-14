/**
 * @file Application.hpp
 * @brief Declares Application, the demo application object whose Run() drives the main render loop.
 */

#pragma once
/**
 * @brief Top-level demo application: owns the main loop that renders a textured cube with a
 * post-processing pass.
 *
 * Constructing an Application performs no setup (defaulted constructor); all
 * resource creation and the render loop live in Run(). Run() builds the window,
 * geometry, shaders, texture and an offscreen framebuffer, then loops until the
 * window is closed, rendering the scene into the framebuffer and applying a
 * full-screen post-processing effect before swapping.
 */
class Application
{
  public:
    /// Constructs the application (no setup performed; see Run()).
    Application();

    /**
     * @brief Runs the main render loop until the window is closed.
     * @return Process exit code (0 on a clean exit).
     */
    int Run();
};
