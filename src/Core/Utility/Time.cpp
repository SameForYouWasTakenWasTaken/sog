/**
 * @file Time.cpp
 * @brief Implements sog::Time::getTime(), forwarding to GLFW's clock.
 *
 */
#include "Time.hpp"

#include "GLFW/glfw3.h"

namespace sog
{

double Time::getTime() { return glfwGetTime(); }
} // namespace sog