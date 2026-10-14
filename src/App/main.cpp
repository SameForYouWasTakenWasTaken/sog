/**
 * @file main.cpp
 * @brief Program entry point: constructs a sog::Application and runs its main loop.
 */

#include "Application.hpp"

/**
 * @brief Entry point of the application.
 * @return Exit code returned by sog::Application::Run() (0 on a clean exit).
 */
int main()
{
    sog::Application app;
    return app.Run();
}
