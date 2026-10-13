#pragma once
#include <unordered_map>

#include "../../Types/GFXTypes.hpp"
#include "Shader.hpp"
#include "gfx/OpenGL/Uniform.hpp"
#include "glad/gl.h"

namespace sog::gfx::backend
{
static void handleProgramError(gl_handle handle, GLenum statusType)
{
    GLint success = GL_FALSE;
    glGetProgramiv(handle, statusType, &success);

    if (!success)
    {
        GLint logLength = 0;
        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &logLength);

        std::string log(logLength, '\0');
        glGetProgramInfoLog(handle, logLength, nullptr, log.data());

        throw graphics_error(graphics_error_code::shader_link_failed, log);
    }
}
} // namespace sog::gfx::backend

namespace sog::gfx
{
class ShaderProgram
{

  public:
    ShaderProgram() { m_Handle = glCreateProgram(); }

    template <shader_type A, shader_type B>
    void Initialize(const Shader<A>& shaderA, const Shader<B>& shaderB);
    void use() const { glUseProgram(m_Handle); }
    [[nodiscard]] gl_handle get() const { return m_Handle; }

    std::optional<Uniform> getLocation(const std::string& name);

  private:
    gl_handle m_Handle{0};

    using uniformName = std::string;
    std::unordered_map<uniformName, Uniform> m_CachedUniforms;
};
template <shader_type A, shader_type B>
void ShaderProgram::Initialize(const Shader<A>& shaderA, const Shader<B>& shaderB)
{
    auto rawA = shaderA.get();
    auto rawB = shaderB.get();
    glAttachShader(m_Handle, rawA);
    glAttachShader(m_Handle, rawB);

    glLinkProgram(m_Handle);

    backend::handleProgramError(m_Handle, GL_LINK_STATUS);
}
} // namespace sog::gfx