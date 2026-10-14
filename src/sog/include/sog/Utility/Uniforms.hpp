/**
 * @file Uniforms.hpp
 * @brief Declares the sog::gfx::uniform free-function API for looking up and uploading typed values
 * to shader uniforms.
 *
 * These helpers use GL's direct-state-access program-uniform entry points
 * (glProgramUniform*), so the program need not be bound first. Each setter
 * validates the uniform's location and throws on an invalid one.
 */

#pragma once
#include <sog/gfx/OpenGL/Objects/ShaderProgram.hpp>
#include <sog/gfx/OpenGL/Uniform.hpp>

#include "Math.hpp"

namespace sog::gfx::uniform
{
/**
 * @brief Looks up a uniform's location by name within a linked program.
 * @param program The linked shader program to query.
 * @param name The uniform's name as declared in the shader.
 * @return Uniform holding the location; may be invalid (-1) if not found.
 *
 * @note Does not throw for a missing uniform — callers get an invalid Uniform to
 * test. Prefer ShaderProgram::getLocation, which caches results per program.
 */
[[nodiscard]] Uniform getLocation(const ShaderProgram& program, const std::string& name);

/**
 * @brief Uploads a 4-component float vector to a uniform.
 * @param program The program owning the uniform.
 * @param uniform Location of the uniform (must be valid).
 * @param vec The (x, y, z, w) values to upload.
 * @throws sog::graphics_error If the uniform location is invalid.
 */
void setVec4(const ShaderProgram& program, Uniform uniform, math::Vec4 vec);

/**
 * @brief Uploads a 3-component float vector to a uniform.
 * @param program The program owning the uniform.
 * @param uniform Location of the uniform (must be valid).
 * @param vec The (x, y, z) values to upload.
 * @throws sog::graphics_error If the uniform location is invalid.
 */
void setVec3(const ShaderProgram& program, Uniform uniform, math::Vec3 vec);

/**
 * @brief Uploads a 2-component float vector to a uniform.
 * @param program The program owning the uniform.
 * @param uniform Location of the uniform (must be valid).
 * @param vec The (x, y) values to upload.
 * @throws sog::graphics_error If the uniform location is invalid.
 */
void setVec2(const ShaderProgram& program, Uniform uniform, math::Vec2 vec);

/**
 * @brief Uploads a 3x3 matrix to a uniform (column-major, no transpose).
 * @param program The program owning the uniform.
 * @param uniform Location of the uniform (must be valid).
 * @param mat The matrix to upload.
 * @throws sog::graphics_error If the uniform location is invalid.
 */
void setMat3(const ShaderProgram& program, Uniform uniform, math::Mat3 mat);

/**
 * @brief Uploads a 4x4 matrix to a uniform (column-major, no transpose).
 * @param program The program owning the uniform.
 * @param uniform Location of the uniform (must be valid).
 * @param mat The matrix to upload.
 * @throws sog::graphics_error If the uniform location is invalid.
 */
void setMat4(const ShaderProgram& program, Uniform uniform, math::Mat4 mat);

/**
 * @brief Uploads an integer to a uniform (commonly sampler bindings).
 * @param program The program owning the uniform.
 * @param uniform Location of the uniform (must be valid).
 * @param value The integer value to upload.
 * @throws sog::graphics_error If the uniform location is invalid.
 */
void setInt(const ShaderProgram& program, Uniform uniform, int value);

/**
 * @brief Uploads a single float to a uniform.
 * @param program The program owning the uniform.
 * @param uniform Location of the uniform (must be valid).
 * @param value The float value to upload.
 * @throws sog::graphics_error If the uniform location is invalid.
 */
void setFloat(const ShaderProgram& program, Uniform uniform, float value);

/**
 * @brief Runs a list of callables, ignoring any exception each one throws.
 *
 * Executes each callback in order inside its own try/catch, so one throwing call
 * does not abort the remaining ones. This is by design: a failing step (e.g. a
 * missing optional uniform) should not stop the rest of the frame.
 *
 * @tparam T Callable types.
 * @param cb Zero or more callables to invoke in sequence.
 */
template <typename... T> void runSafe(T&&... cb)
{
    (
        [&]
        {
            try
            {
                std::forward<T>(cb)();
            }
            catch (const std::exception& e)
            {
                // Handle exception
            }
        }(),
        ...);
}
} // namespace sog::gfx::uniform
