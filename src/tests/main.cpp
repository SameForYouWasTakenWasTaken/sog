/**
 * @file main.cpp
 * @brief Custom GoogleTest entry point: initializes GLFW, forces hidden windows so every OpenGL
 * context the suite creates is off-screen, brings up the shared context, runs all tests, then
 * tears everything down.
 *
 * This replaces gtest_main so GLFW is initialized exactly once, before any test creates a Window.
 * Setting GLFW_VISIBLE to false here makes every subsequent glfwCreateWindow (including the
 * engine's Window::initialize, which does not set GLFW_VISIBLE) produce a hidden window, so the
 * suite runs correctly on headless CI and never flashes a window on a developer's desktop.
 *
 * The shared context is created eagerly so that plain TEST() cases also run with a current
 * context (CTest invokes each case in a fresh process). The --gtest_list_tests pass that
 * gtest_discover_tests performs at build time skips GL initialization entirely: listing must not
 * require a GPU.
 */

#include <gtest/gtest.h>

#include <GLFW/glfw3.h>

#include <string_view>

#include "TestSupport.hpp"

namespace
{
/// Reports whether the arguments ask GoogleTest to only list tests (build-time discovery).
bool isListingTests(int argc, char** argv)
{
    for (int i = 1; i < argc; ++i)
        if (std::string_view{argv[i]} == "--gtest_list_tests")
            return true;
    return false;
}
} // namespace

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    const bool listing = isListingTests(argc, argv);

    if (!listing)
    {
        if (glfwInit() != GLFW_TRUE)
        {
            std::fputs("FATAL: glfwInit() failed; the SOG test suite cannot create a context.\n",
                       stderr);
            return EXIT_FAILURE;
        }

        // Hidden windows for every context created from here on (shared context and per-test ones).
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        // Create + make current the shared context every GL-touching test relies on.
        (void)sogtest::sharedContext();
    }

    const int result = RUN_ALL_TESTS();

    // Release the shared context's GL objects and its window before terminating GLFW.
    sogtest::destroySharedContext();
    if (!listing)
        glfwTerminate();

    return result;
}
