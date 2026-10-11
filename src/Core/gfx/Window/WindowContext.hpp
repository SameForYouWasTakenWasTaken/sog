#pragma once
#include <cstdint>
#include <string>

namespace sog::context
{
struct WindowContext
{
    enum class OpenGLProfile : int
    {
        CORE,
        COMPATIBILITY,
    };
    int OpenGL_Version_Major = 4;
    int OpenGL_Version_Minor = 6;
    OpenGLProfile OpenGL_Profile = OpenGLProfile::CORE;

    int width = 800;
    int height = 600;
    std::string title{};
};
} // namespace sog::context