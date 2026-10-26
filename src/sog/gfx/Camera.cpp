/**
 * @file Camera.cpp
 * @brief Implements Camera: the frame-quaternion construction from world-up, basis-vector
 * derivation, pose/projection mutators and the cached view/projection matrices.
 */

#include <algorithm>
#include <cmath>
#include <sog/gfx/Camera.hpp>

namespace sog::gfx
{
namespace
{
// Rotation that maps +Y onto 'up'
/**
 * @brief Builds the frame quaternion rotating +Y onto the given up vector.
 * @param up Target up direction (assumed normalized and non-zero).
 * @return Quaternion: identity when up is already +Y, a 180-degree flip about X
 * when up is -Y, otherwise a minimal rotation about the perpendicular axis.
 */
glm::quat frameFromUp(const math::Vec3& up)
{
    const math::Vec3 y{0.f, 1.f, 0.f};
    const float d = glm::dot(y, up);
    if (d > 0.9999f)
        return glm::quat{1.f, 0.f, 0.f, 0.f};
    if (d < -0.9999f)
        return glm::angleAxis(glm::pi<float>(), math::Vec3{1.f, 0.f, 0.f});
    return glm::angleAxis(std::acos(d), glm::normalize(glm::cross(y, up)));
}
} // namespace

Camera::Camera(math::Vec3 worldUp)
    : m_WorldUp{glm::normalize(worldUp)}, m_Frame{frameFromUp(m_WorldUp)}
{
}

Camera::Basis Camera::basis() const
{
    const float yaw = glm::radians(m_Yaw), pitch = glm::radians(m_Pitch);

    const math::Vec3 f = m_Frame * math::Vec3{std::cos(pitch) * std::cos(yaw), std::sin(pitch),
                                              std::cos(pitch) * std::sin(yaw)};
    // Depends only on yaw, so it stays valid at any pitch
    math::Vec3 r = m_Frame * math::Vec3{-std::sin(yaw), 0.f, std::cos(yaw)};
    math::Vec3 u = glm::cross(r, f);

    if (m_Roll != 0.f)
    {
        const glm::quat roll = glm::angleAxis(glm::radians(m_Roll), f);
        r = roll * r;
        u = roll * u;
    }
    return {f, r, u};
}

math::Vec3 Camera::getForward() const { return basis().forward; }
math::Vec3 Camera::getRight() const { return basis().right; }
math::Vec3 Camera::getUp() const { return basis().up; }

// ---- Pose ----
void Camera::setPosition(math::Vec3 p)
{
    m_Position = p;
    m_ViewDirty = true;
}
void Camera::move(math::Vec3 d)
{
    m_Position += d;
    m_ViewDirty = true;
}

void Camera::moveLocal(float right, float up, float forward)
{
    const Basis b = basis();
    move(b.right * right + b.up * up + b.forward * forward);
}

void Camera::setRotation(float yawDeg, float pitchDeg, float rollDeg)
{
    m_Yaw = yawDeg;
    m_Pitch = pitchDeg;
    m_Roll = rollDeg;
    m_ViewDirty = true;
}

void Camera::rotateBy(float yaw, float pitch, float roll)
{
    setRotation(m_Yaw + yaw, m_Pitch + pitch, m_Roll + roll);
}

void Camera::lookAt(math::Vec3 eye, math::Vec3 target)
{
    m_Position = eye;
    const math::Vec3 d = glm::inverse(m_Frame) * glm::normalize(target - eye);
    setRotation(glm::degrees(std::atan2(d.z, d.x)), glm::degrees(std::asin(d.y)), 0.f);
}

// ---- Projection ----
void Camera::setPerspective(float fov, float aspect, float n, float f)
{
    m_Type = ProjectionType::Perspective;
    m_Fov = fov;
    m_Aspect = aspect;
    m_Near = n;
    m_Far = f;
    m_ProjDirty = true;
}

void Camera::setOrthographic(float l, float r, float b, float t, float n, float f)
{
    m_Type = ProjectionType::Orthographic;
    m_Left = l;
    m_Right = r;
    m_Bottom = b;
    m_Top = t;
    m_Near = n;
    m_Far = f;
    m_ProjDirty = true;
}

void Camera::setAspect(float a)
{
    m_Aspect = a;
    m_ProjDirty = true;
}
void Camera::setFov(float fov)
{
    m_Fov = fov;
    m_ProjDirty = true;
}
void Camera::setClipPlanes(float n, float f)
{
    m_Near = n;
    m_Far = f;
    m_ProjDirty = true;
}

// ---- Matrices ----
math::Mat4 Camera::getRotationMatrix() const
{
    const Basis b = basis();
    math::Mat4 rotation{1.f};
    // GLM matrices are indexed by column.
    rotation[0] = math::Vec4{b.right, 0.f};
    rotation[1] = math::Vec4{b.up, 0.f};
    rotation[2] = math::Vec4{-b.forward, 0.f};
    return rotation;
}

const math::Mat4& Camera::getView() const
{
    if (m_ViewDirty)
    {
        const Basis b = basis();
        m_View = glm::lookAt(m_Position, m_Position + b.forward, b.up);
        m_ViewDirty = false;
    }
    return m_View;
}

const math::Mat4& Camera::getProjection() const
{
    if (m_ProjDirty)
    {
        m_Projection = (m_Type == ProjectionType::Perspective)
                           ? glm::perspective(glm::radians(m_Fov), m_Aspect, m_Near, m_Far)
                           : glm::ortho(m_Left, m_Right, m_Bottom, m_Top, m_Near, m_Far);
        m_ProjDirty = false;
    }
    return m_Projection;
}
} // namespace sog::gfx