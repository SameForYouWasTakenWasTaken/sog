/**
 * @file Math.hpp
 * @brief Re-exports the GLM math types the engine uses under the sog::math namespace
 * (Vec2/Vec3/Vec4, Mat3/Mat4, glm::value_ptr).
 *
 * All engine code should refer to sog::math::Vec3 and friends rather than glm
 * directly, so the underlying math library stays replaceable from one place.
 */

#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace sog::math
{
inline namespace types
{
/// 2-component single-precision vector (alias of glm::vec2).
using Vec2 = glm::vec2;
/// 3-component single-precision vector (alias of glm::vec3).
using Vec3 = glm::vec3;
/// 4-component single-precision vector (alias of glm::vec4).
using Vec4 = glm::vec4;
/// 4x4 single-precision column-major matrix, the engine's standard transform matrix (alias of
/// glm::mat4).
using Mat4 = glm::mat4;
/// 3x3 single-precision matrix, typically used for normal/inverse-transpose transforms (alias of
/// glm::mat3).
using Mat3 = glm::mat3;

using glm::value_ptr; /**< Re-exports glm::value_ptr for uploading matrix/vector data to OpenGL,
                         e.g. glUniformMatrix4fv(loc, 1, GL_FALSE, math::value_ptr(mvp)). */
} // namespace types
} // namespace sog::math
