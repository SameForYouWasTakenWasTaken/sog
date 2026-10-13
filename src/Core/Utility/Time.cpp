#pragma once
#include "Time.hpp"

#include "GLFW/glfw3.h"

namespace sog
{

double Time::getTime() { return glfwGetTime(); }
} // namespace sog