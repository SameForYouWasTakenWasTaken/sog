#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Types/Math.hpp"

namespace sog::gfx
{
class Camera
{
  public:
    enum class ProjectionType
    {
        Perspective,
        Orthographic
    };

    explicit Camera(math::Vec3 worldUp = {0.f, 1.f, 0.f});

    // Pose
    void setPosition(math::Vec3 position);
    void move(math::Vec3 delta);                          // world space
    void moveLocal(float right, float up, float forward); // camera space
    void setRotation(float yawDeg, float pitchDeg, float rollDeg = 0.f);
    void rotateBy(float yawDelta, float pitchDelta, float rollDelta = 0.f);
    void lookAt(math::Vec3 eye, math::Vec3 target);

    // Projection
    void setPerspective(float fovDeg, float aspect, float nearPlane, float farPlane);
    void setOrthographic(float left, float right, float bottom, float top, float nearPlane,
                         float farPlane);
    void setAspect(float aspect); // call on window resize (perspective only)
    void setFov(float fovDeg);
    void setClipPlanes(float nearPlane, float farPlane);

    // Getters
    [[nodiscard]] math::Vec3 getPosition() const { return m_Position; }
    [[nodiscard]] math::Vec3 getForward() const;
    [[nodiscard]] math::Vec3 getRight() const;
    [[nodiscard]] math::Vec3 getUp() const;
    [[nodiscard]] float getYaw() const { return m_Yaw; }
    [[nodiscard]] float getPitch() const { return m_Pitch; }
    [[nodiscard]] float getRoll() const { return m_Roll; }
    [[nodiscard]] ProjectionType getProjectionType() const { return m_Type; }

    [[nodiscard]] const math::Mat4& getView() const;
    [[nodiscard]] const math::Mat4& getProjection() const;
    [[nodiscard]] math::Mat4 getViewProjection() const { return getProjection() * getView(); }

  private:
    struct Basis
    {
        math::Vec3 forward, right, up;
    };
    [[nodiscard]] Basis basis() const;

    // Pose (angles in degrees, yaw/pitch are measured in a "Y-up" space and
    // rotated into the user's up convention by m_Frame)
    math::Vec3 m_WorldUp;
    glm::quat m_Frame{1.f, 0.f, 0.f, 0.f};
    math::Vec3 m_Position{0.f, 0.f, 3.f};
    float m_Yaw{-90.f}, m_Pitch{0.f}, m_Roll{0.f};

    // Projection
    ProjectionType m_Type{ProjectionType::Perspective};
    float m_Fov{60.f}, m_Aspect{16.f / 9.f}, m_Near{0.1f}, m_Far{100.f};
    float m_Left{-1.f}, m_Right{1.f}, m_Bottom{-1.f}, m_Top{1.f};

    // Cached matrices
    mutable math::Mat4 m_View{1.f}, m_Projection{1.f};
    mutable bool m_ViewDirty{true}, m_ProjDirty{true};
};
} // namespace sog::gfx
