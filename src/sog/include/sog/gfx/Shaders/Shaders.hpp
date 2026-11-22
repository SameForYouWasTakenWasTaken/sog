#pragma once

#include "sog/gfx/OpenGL/Objects/ShaderProgram.hpp"

namespace sog::gfx::shaders
{
/**
 * @brief Creates the default vertex-color shader program.
 *
 * Uses the standard sog::Vertex attribute layout:
 * - Location 0: Vertex position (vec3).
 * - Location 1: Vertex color (vec4).
 * - Location 2: Texture coordinates (vec2, unused).
 *
 * Vertex positions are passed directly to clip space.
 * Vertex colors are interpolated across the primitive.
 *
 * @return A linked shader program.
 *
 * @note No uniforms are required.
 */
ShaderProgram getCommonShader();

/**
 * @brief Creates a vertex-color shader program with MVP transformations.
 *
 * Uses the standard sog::Vertex attribute layout:
 * - Location 0: Vertex position (vec3).
 * - Location 1: Vertex color (vec4).
 * - Location 2: Texture coordinates (vec2, unused).
 *
 * Vertex positions are transformed before being passed to clip space.
 *
 * @return A linked shader program.
 *
 * @note Requires the following uniforms:
 * - uMVP (mat4): Model-View-Projection transformation matrix.
 *
 * @note Expected transformation order: Projection * View * Model.
 */
ShaderProgram getMVPShader();

/**
 * @brief Creates a shader program that renders geometry using a uniform color.
 *
 * Uses the following vertex attribute layout:
 * - Location 0: Vertex position (vec3).
 *
 * @return A linked shader program.
 *
 * @note Requires the following uniforms:
 * - uMVP (mat4): Model-View-Projection transformation matrix.
 * - uColor (vec4): RGBA color applied to the entire primitive.
 */
ShaderProgram getSolidColorShader();

/**
 * @brief Creates a textured vertex-color shader program.
 *
 * Uses the standard sog::Vertex attribute layout:
 * - Location 0: Vertex position (vec3).
 * - Location 1: Vertex color (vec4).
 * - Location 2: Texture coordinates (vec2).
 *
 * Samples a 2D texture and multiplies its color by the interpolated
 * vertex color.
 *
 * @return A linked shader program.
 *
 * @note Requires the following uniforms:
 * - uMVP (mat4): Model-View-Projection transformation matrix.
 * - uTexture (sampler2D): Texture sampled using the interpolated UV coordinates.
 *
 * @note uTexture expects a texture unit index, not a texture object handle.
 */
ShaderProgram getTextureShader();

/**
 * @brief Creates a shader program for instanced rendering.
 *
 * Uses the following vertex attribute layout:
 * - Location 0: Vertex position (vec3).
 * - Location 1: Instance color (vec4).
 * - Locations 2-5: Instance transformation matrix (mat4).
 *
 * Each instance provides its own color and transformation matrix.
 *
 * @return A linked shader program.
 *
 * @note Requires the following uniforms:
 * - uViewProjection (mat4): Combined view-projection transformation matrix.
 *
 * @note Instance color and transformation attributes should use a divisor of 1.
 * @note The mat4 occupies four consecutive attribute locations (2-5).
 */
ShaderProgram getInstancedShader();

/**
 * @brief Creates a shader program that visualizes vertex normals.
 *
 * Uses the following vertex attribute layout:
 * - Location 0: Vertex position (vec3).
 * - Location 1: Vertex normal (vec3).
 *
 * Transforms normals using a normal matrix and displays them as RGB colors.
 *
 * @return A linked shader program.
 *
 * @note Requires the following uniforms:
 * - uMVP (mat4): Model-View-Projection transformation matrix.
 * - uNormalMatrix (mat3): Inverse-transpose of the model matrix's upper-left 3x3.
 *
 * @note Intended for debugging geometry and normal transformations.
 */
ShaderProgram getNormalVisualizationShader();

/**
 * @brief Creates a depth-only shader program.
 *
 * Uses a vertex position attribute:
 * - Location 0: Vertex position (vec3).
 *
 * Transforms geometry using an MVP matrix. The fragment shader does not
 * produce a color output, allowing the depth buffer to be populated.
 *
 * @return A linked shader program.
 *
 * @note Requires the following uniforms:
 * - uMVP (mat4): Transformation matrix, typically light-space MVP for shadow mapping.
 *
 * @note Requires a framebuffer with a depth attachment.
 */
ShaderProgram getDepthShader();

/**
 * @brief Creates a screen-space texture shader program.
 *
 * Uses the following vertex attribute layout:
 * - Location 0: Vertex position (vec2).
 * - Location 1: Texture coordinates (vec2).
 *
 * Samples a 2D texture and writes its color directly to the fragment output.
 *
 * @return A linked shader program.
 *
 * @note Requires the following uniforms:
 * - uTexture (sampler2D): Texture sampled using the interpolated UV coordinates.
 *
 * @note uTexture expects a texture unit index, not a texture object handle.
 * @note Intended for fullscreen quads and post-processing.
 */
ShaderProgram getScreenSpaceShader();

} // namespace sog::gfx::shaders