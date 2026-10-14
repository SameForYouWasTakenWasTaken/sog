/**
 * @file main.cpp
 * @brief Program entry point: constructs an Application and runs its main loop.
 */

#include "Application.hpp"

/**
 * @brief Entry point of the application.
 * @return Exit code returned by Application::Run() (0 on a clean exit).
 */
int main()
{
    Application app;
    return app.Run();
}
