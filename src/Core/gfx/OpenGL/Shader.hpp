#pragma once
#include <filesystem>
#include <fstream>
#include <string>

#include "Error/Exceptions/graphics_error.hpp"
#include "Util.hpp"
#include "gfx/Types/Enums.hpp"
#include "gfx/Types/GFXTypes.hpp"
#include "gfx/Types/Handles.hpp"

namespace sog::gfx::backend
{
static void handleShaderError(gl_handle handle, GLenum statusType)
{
    GLint success = GL_FALSE;
    glGetShaderiv(handle, statusType, &success);

    if (!success)
    {
        GLint logLength = 0;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLength);

        std::string log(logLength, '\0');
        glGetShaderInfoLog(handle, logLength, nullptr, log.data());

        throw graphics_error(graphics_error_code::shader_compilation_failed, log);
    }
}
} // namespace sog::gfx::backend

namespace sog::gfx
{
template <shader_type> class Shader
{
  public:
    Shader() = default;
    ~Shader() { glDeleteShader(m_Handle.get()); }

    void createFromSource(std::string_view source);
    void createFromFile(const std::filesystem::path& path);
    [[nodiscard]] gl_handle get() const { return m_Handle.get(); }

  private:
    void compile() const;
    ShaderHandle m_Handle{};
};
template <shader_type TShaderType>
void Shader<TShaderType>::createFromSource(std::string_view source)
{
    if (!m_Handle.valid())
        m_Handle.raw() = glCreateShader(static_cast<GLenum>(TShaderType));
    const GLchar* src = source.data();
    const auto length = static_cast<GLint>(source.size());
    glShaderSource(m_Handle.get(), 1, &src, &length);

    compile();
}
template <shader_type E0> void Shader<E0>::createFromFile(const std::filesystem::path& path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw graphics_error(graphics_error_code::shader_file_open_failed,
                             "Could not open shader! file: " + path.string());

    const std::string source{std::istreambuf_iterator<char>{file},
                             std::istreambuf_iterator<char>{}};

    createFromSource(source);
}
template <shader_type TShaderType> void Shader<TShaderType>::compile() const
{
    glCompileShader(m_Handle.get());
    backend::handleShaderError(m_Handle.get(), GL_COMPILE_STATUS);
}

using VertexShader = Shader<shader_type::VERTEX>;
using FragmentShader = Shader<shader_type::FRAGMENT>;
} // namespace sog::gfx