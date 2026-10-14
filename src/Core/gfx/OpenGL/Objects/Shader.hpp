/**
 * @file Shader.hpp
 * @brief Defines the Shader<Stage> template (compile a GL shader stage from source or file), plus the VertexShader and FragmentShader aliases and a compile-status error helper.
 */

#pragma once
#include <filesystem>
#include <fstream>
#include <string>

#include "Error/Exceptions/graphics_error.hpp"
#include "gfx/Types/Enums.hpp"
#include "gfx/Types/GFXTypes.hpp"
#include "gfx/Types/Handles.hpp"

namespace sog::gfx::backend
{
/**
 * @brief Checks a shader's compile/link status and throws with the info log on failure.
 * @param handle The shader (or program) object to inspect.
 * @param statusType Query for the outcome, e.g. GL_COMPILE_STATUS or GL_LINK_STATUS.
 * @throws sog::graphics_error With the GL info log when the status query fails.
 */
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
/**
 * @brief A single compiled GL shader stage, templated on its shader_type.
 *
 * Owns one GL shader object (identified by a ShaderHandle). Call createFromSource()
 * or createFromFile() to compile; either throws sog::graphics_error if compilation
 * fails (with the driver's info log) or the file cannot be opened. The GL object is
 * deleted on destruction.
 *
 * The stage is fixed at compile time by the template parameter, so a VertexShader
 * can only hold vertex code, a FragmentShader only fragment code, and so on.
 *
 * @tparam Stage Which shader stage this object represents (from sog::shader_type).
 *
 * @note Compile status is checked after each create* call, so a Shader is always
 * either freshly compiled or has already thrown.
 *
 * @see sog::VertexShader, sog::FragmentShader, sog::ShaderProgram
 */
template <shader_type> class Shader
{
  public:
    /// Constructs an empty shader with no GL object yet.
    Shader() = default;

    /// Deletes the owned GL shader object.
    ~Shader() { glDeleteShader(m_Handle.get()); }

    /**
     * @brief Compiles the shader from an in-memory source string.
     * @param source GLSL source text for this stage.
     * @throws sog::graphics_error If compilation fails (info log included in the message).
     */
    void createFromSource(std::string_view source);

    /**
     * @brief Reads a file and compiles the shader from its contents.
     * @param path Path to the GLSL source file.
     * @throws sog::graphics_error If the file cannot be opened or compilation fails.
     */
    void createFromFile(const std::filesystem::path& path);

    /**
     * @brief Returns the raw GL shader name.
     * @return GLuint handle of the shader object.
     */
    [[nodiscard]] gl_handle get() const { return m_Handle.get(); }

  private:
    /**
     * @brief Calls glCompileShader and validates the result, throwing on failure.
     */
    void compile() const;

    ShaderHandle m_Handle{}; /**< Typed handle to the owned GL shader object. */
};

/**
 * @brief Compiles the shader from source, creating the GL object on first use.
 * @param source GLSL source text for this stage.
 * @throws sog::graphics_error If compilation fails.
 */
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

/**
 * @brief Reads the file into memory and compiles it via createFromSource().
 * @param path Path to the GLSL source file.
 * @throws sog::graphics_error With graphics_error_code::shader_file_open_failed if the file cannot be opened.
 */
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

/**
 * @brief Compiles the current source and validates GL_COMPILE_STATUS.
 * @throws sog::graphics_error If compilation fails, with the driver info log.
 */
template <shader_type TShaderType> void Shader<TShaderType>::compile() const
{
    glCompileShader(m_Handle.get());
    backend::handleShaderError(m_Handle.get(), GL_COMPILE_STATUS);
}

/// Shader specialized for the vertex stage (shader_type::VERTEX).
using VertexShader = Shader<shader_type::VERTEX>;
/// Shader specialized for the fragment stage (shader_type::FRAGMENT).
using FragmentShader = Shader<shader_type::FRAGMENT>;
} // namespace sog::gfx
