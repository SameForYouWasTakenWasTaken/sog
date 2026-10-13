#pragma once
#include "Objects/ShaderProgram.hpp"
#include "Types/Math.hpp"

namespace sog::gfx::uniform
{
struct Uniform
{
    using Location = GLint;
    Location location = -1;

    [[nodiscard]] constexpr bool valid() const noexcept { return location != -1; }
    [[nodiscard]] constexpr explicit operator bool() const noexcept { return valid(); }
    [[nodiscard]] constexpr explicit operator Location() const noexcept { return location; }
};
[[nodiscard]] Uniform getLocation(const ShaderProgram& program, const std::string& name);
void setVec4(const ShaderProgram& program, Uniform uniform, math::Vec4 vec);
void setVec3(const ShaderProgram& program, Uniform uniform, math::Vec3 vec);
void setVec2(const ShaderProgram& program, Uniform uniform, math::Vec2 vec);

void setMat3(const ShaderProgram& program, Uniform uniform, math::Mat3 mat);
void setMat4(const ShaderProgram& program, Uniform uniform, math::Mat4 mat);

void setInt(const ShaderProgram& program, Uniform uniform, int value);
void setFloat(const ShaderProgram& program, Uniform uniform, float value);

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