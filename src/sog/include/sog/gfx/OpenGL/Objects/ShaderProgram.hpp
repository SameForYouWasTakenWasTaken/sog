/**
 * @file ShaderProgram.hpp
 * @brief Defines ShaderProgram, a linked GL program that attaches shader stages, binds for drawing and caches uniform locations.
 */

#pragma once
#include <unordered_map>

#include "../../Types/GFXTypes.hpp"
#include "../Uniform.hpp"
#include "Shader.hpp"
#include "glad/gl.h"

namespace sog::gfx::backend
{
/**
 * @brief Checks a program's link status and throws with the info log on failure.
 * @param handle The program to inspect.
 * @param statusType Query for the outcome, typically GL_LINK_STATUS.
 * @throws sog::graphics_error With the GL info log when the status query fails.
 */
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
/**
 * @brief A linked OpenGL shader program owning two shader stages.
 *
 * The constructor creates the GL program object. Initialize() attaches two
 * compiled shaders (of any two distinct stage types), links the program and
 * validates the link, throwing sog::graphics_error with the driver's info log on
 * failure. use() makes the program current for drawing.
 *
 * Uniform locations are looked up through getLocation(), which caches results by
 * name so repeated per-frame uniform updates avoid redundant glGetUniformLocation
 * calls.
 *
 * @attention The program is created eagerly in the constructor but is only usable
 * after Initialize() links it. The attached shader objects remain owned by the
 * caller.
 *
 * @see sog::Shader, sog::Uniform
 */
class ShaderProgram
{

  public:
    /// Creates the GL program object (glCreateProgram).
    ShaderProgram() { m_Handle = glCreateProgram(); }

    /**
     * @brief Attaches two compiled shaders and links the program.
     * @param shaderA First shader stage (any shader_type).
     * @param shaderB Second shader stage (any shader_type).
     * @throws sog::graphics_error With graphics_error_code::shader_link_failed if linking fails.
     */
    template <shader_type A, shader_type B>
    void Initialize(const Shader<A>& shaderA, const Shader<B>& shaderB);

    /**
     * @brief Makes this program current for subsequent draw calls (glUseProgram).
     */
    void use() const { glUseProgram(m_Handle); }

    /**
     * @brief Returns the raw GL program name.
     * @return GLuint handle of the program object.
     */
    [[nodiscard]] gl_handle get() const { return m_Handle; }

    /**
     * @brief Returns a uniform's location, caching lookups by name.
     * @param name The uniform's name as declared in the shader.
     * @return The cached or freshly looked-up Uniform; invalid (-1) if not found.
     */
    std::optional<Uniform> getLocation(const std::string& name);

  private:
    gl_handle m_Handle{0}; /**< Raw GL program name. */

    /// Convenience alias for uniform cache keys.
    using uniformName = std::string;
    std::unordered_map<uniformName, Uniform> m_CachedUniforms; /**< name -> Uniform cache to skip repeated glGetUniformLocation calls. */
};

/**
 * @brief Attaches both shader handles to the program, links it, and validates the result.
 * @param shaderA First stage to attach.
 * @param shaderB Second stage to attach.
 * @throws sog::graphics_error If linking fails, carrying the driver info log.
 */
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
