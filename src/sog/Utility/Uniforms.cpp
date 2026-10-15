/**
 * @file Uniforms.cpp
 * @brief Implements the sog::gfx::uniform helpers: uniform lookup plus typed uploads via GL's
 * direct-state-access glProgramUniform* entry points.
 */

#include <sog/Utility/Uniforms.hpp>

#include "glad/gl.h"

namespace sog::gfx::uniform::backend
{
/**
 * @brief Throws if the given shader program or uniform location is invalid.
 * @param programHandle Program whose GL handle must be valid (non-zero).
 * @param uniform Location to validate.
 * @throws sog::graphics_error With graphics_error_code::uniform_location_invalid when !valid(),
 * or graphics_error_code::invalid_shader_program when the program handle is invalid.
 */
void sanityCheck(const ShaderProgramHandle& programHandle, Uniform uniform)
{
    if (!uniform.valid())
        throw graphics_error(graphics_error_code::uniform_location_invalid,
                             "Uniform location is invalid!");
    if (!programHandle.valid())
        throw graphics_error(graphics_error_code::invalid_shader_program,
                             "Shader program is invalid!");
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
    backend::sanityCheck(program.get(), uniform);
    glProgramUniform4f(program.get(), uniform.location, vec.x, vec.y, vec.z, vec.w);
}

void setVec3(const ShaderProgram& program, Uniform uniform, math::Vec3 vec)
{
    backend::sanityCheck(program.get(), uniform);
    glProgramUniform3f(program.get(), uniform.location, vec.x, vec.y, vec.z);
}

void setVec2(const ShaderProgram& program, Uniform uniform, math::Vec2 vec)
{
    backend::sanityCheck(program.get(), uniform);
    glProgramUniform2f(program.get(), uniform.location, vec.x, vec.y);
}

void setMat3(const ShaderProgram& program, Uniform uniform, math::Mat3 mat)
{
    backend::sanityCheck(program.get(), uniform);
    glProgramUniformMatrix3fv(program.get(), uniform.location, 1, GL_FALSE, math::value_ptr(mat));
}
void setMat4(const ShaderProgram& program, Uniform uniform, math::Mat4 mat)
{
    backend::sanityCheck(program.get(), uniform);
    glProgramUniformMatrix4fv(program.get(), uniform.location, 1, GL_FALSE, math::value_ptr(mat));
}
void setInt(const ShaderProgram& program, Uniform uniform, int value)
{
    backend::sanityCheck(program.get(), uniform);
    glProgramUniform1i(program.get(), uniform.location, value);
}
void setFloat(const ShaderProgram& program, Uniform uniform, float value)
{
    backend::sanityCheck(program.get(), uniform);
    glProgramUniform1f(program.get(), uniform.location, value);
}
} // namespace sog::gfx::uniform