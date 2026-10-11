#include "Uniforms.hpp"

#include "glad/gl.h"

namespace sog::gfx::uniform::backend
{
void sanityCheck(Uniform uniform)
{
    if (!uniform.valid())
        throw graphics_error(graphics_error_code::uniform_location_invalid,
                             "Uniform location is invalid!");
}
} // namespace sog::gfx::uniform::backend
namespace sog::gfx::uniform
{

Uniform getLocation(const ShaderProgram& program, const std::string& name)
{
    return Uniform{glGetUniformLocation(program.get(), name.c_str())};
}

void setVec4(const ShaderProgram& program, Uniform uniform, math::Vec4 vec)
{
    backend::sanityCheck(uniform);
    glProgramUniform4f(program.get(), uniform.location, vec.x, vec.y, vec.z, vec.w);
}

void setVec3(const ShaderProgram& program, Uniform uniform, math::Vec3 vec)
{
    backend::sanityCheck(uniform);
    glProgramUniform3f(program.get(), uniform.location, vec.x, vec.y, vec.z);
}

void setVec2(const ShaderProgram& program, Uniform uniform, math::Vec2 vec)
{
    backend::sanityCheck(uniform);
    glProgramUniform2f(program.get(), uniform.location, vec.x, vec.y);
}

void setMat3(const ShaderProgram& program, Uniform uniform, math::Mat3 mat)
{
    backend::sanityCheck(uniform);
    glProgramUniformMatrix3fv(program.get(), uniform.location, 1, GL_FALSE, math::value_ptr(mat));
}
void setMat4(const ShaderProgram& program, Uniform uniform, math::Mat4 mat)
{
    backend::sanityCheck(uniform);
    glProgramUniformMatrix4fv(program.get(), uniform.location, 1, GL_FALSE, math::value_ptr(mat));
}
void setInt(const ShaderProgram& program, Uniform uniform, int value)
{
    backend::sanityCheck(uniform);
    glProgramUniform1i(program.get(), uniform.location, value);
}
} // namespace sog::gfx::uniform