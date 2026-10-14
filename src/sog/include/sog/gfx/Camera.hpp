/**
 * @file Camera.hpp
 * @brief Defines Camera, a fly-style camera with yaw/pitch/roll pose controls,
 * perspective/orthographic projection and lazily cached view/projection matrices.
 */

#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../Utility/Math.hpp"

namespace sog::gfx
{
/**
 * @brief A movable, rotatable camera producing view and projection matrices.
 *
 * The camera stores a position and Euler-style orientation (yaw, pitch, roll in
 * degrees) plus a world-up reference that lets it work in any up-convention via an
 * internal frame quaternion. Projection can be perspective (setPerspective) or
 * orthographic (setOrthographic); the active type is remembered and the matching
 * matrix is built on demand.
 *
 * Both getView() and getProjection() cache their result and rebuild only after a
 * corresponding mutator marks them dirty, so querying matrices every frame is
 * cheap.
 *
 * Movement comes in two flavours: move() translates in world space, while
 * moveLocal() translates along the camera's own right/up/forward axes.
 * lookAt() repositions and reorients the camera to gaze from eye toward target.
 *
 * @note Angles are in degrees. yaw == -90 and pitch == 0 (the defaults) make the
 * camera look along -Z in the default frame.
 *
 * @see sog::math::Mat4
 */
class Camera
{
  public:
    /**
     * @brief Which projection matrix the camera currently builds.
     */
    enum class ProjectionType
    {
        Perspective, /**< Perspective (foreshortening) projection via glm::perspective. */
        Orthographic /**< Orthographic (parallel) projection via glm::ortho. */
    };

    /**
     * @brief Constructs a camera with an optional world-up direction.
     * @param worldUp Reference up direction; defaults to +Y. Must not be zero.
     */
    explicit Camera(math::Vec3 worldUp = {0.f, 1.f, 0.f});

    // ---- Pose ----

    /**
     * @brief Teleports the camera to a world-space position.
     * @param position New world-space location.
     */
    void setPosition(math::Vec3 position);

    /**
     * @brief Translates the camera by a world-space offset.
     * @param delta Offset added to the current position (world axes).
     */
    void move(math::Vec3 delta); // world space

    /**
     * @brief Translates the camera along its own axes.
     * @param right Distance to move along the camera's right axis.
     * @param up Distance to move along the camera's up axis.
     * @param forward Distance to move along the camera's forward axis.
     */
    void moveLocal(float right, float up, float forward); // camera space

    /**
     * @brief Sets the orientation from absolute yaw/pitch/roll angles.
     * @param yawDeg Rotation about the up axis, in degrees.
     * @param pitchDeg Elevation above the horizontal plane, in degrees.
     * @param rollDeg Rotation about the forward axis, in degrees (default 0).
     */
    void setRotation(float yawDeg, float pitchDeg, float rollDeg = 0.f);

    /**
     * @brief Rotates the camera by relative yaw/pitch/roll increments.
     * @param yawDelta Degrees to add to the current yaw.
     * @param pitchDelta Degrees to add to the current pitch.
     * @param rollDelta Degrees to add to the current roll (default 0).
     */
    void rotateBy(float yawDelta, float pitchDelta, float rollDelta = 0.f);

    /**
     * @brief Positions and orients the camera to look from eye toward target.
     * @param eye New world-space position of the camera.
     * @param target World-space point to look at; must differ from eye.
     */
    void lookAt(math::Vec3 eye, math::Vec3 target);

    // ---- Projection ----

    /**
     * @brief Switches to a perspective projection with the given parameters.
     * @param fovDeg Vertical field of view in degrees.
     * @param aspect Width/height aspect ratio.
     * @param nearPlane Distance to the near clip plane.
     * @param farPlane Distance to the far clip plane.
     */
    void setPerspective(float fovDeg, float aspect, float nearPlane, float farPlane);

    /**
     * @brief Switches to an orthographic projection with the given frustum bounds.
     * @param left Left clip-plane boundary.
     * @param right Right clip-plane boundary.
     * @param bottom Bottom clip-plane boundary.
     * @param top Top clip-plane boundary.
     * @param nearPlane Distance to the near clip plane.
     * @param farPlane Distance to the far clip plane.
     */
    void setOrthographic(float left, float right, float bottom, float top, float nearPlane,
                         float farPlane);

    /**
     * @brief Updates the aspect ratio (perspective projections only).
     * @param aspect New width/height ratio; call on window resize.
     */
    void setAspect(float aspect); // call on window resize (perspective only)

    /**
     * @brief Updates the perspective field of view.
     * @param fovDeg New vertical field of view in degrees.
     */
    void setFov(float fovDeg);

    /**
     * @brief Updates both clip-plane distances for the current projection.
     * @param nearPlane New near clip-plane distance.
     * @param farPlane New far clip-plane distance.
     */
    void setClipPlanes(float nearPlane, float farPlane);

    // ---- Getters ----

    /**
     * @brief Returns the camera's world-space position.
     * @return Current position vector.
     */
    [[nodiscard]] math::Vec3 getPosition() const { return m_Position; }

    /**
     * @brief Returns the camera's forward direction (unit vector).
     * @return Forward basis vector derived from yaw/pitch/roll and the frame.
     */
    [[nodiscard]] math::Vec3 getForward() const;

    /**
     * @brief Returns the camera's right direction (unit vector).
     * @return Right basis vector (yaw-only, valid at any pitch).
     */
    [[nodiscard]] math::Vec3 getRight() const;

    /**
     * @brief Returns the camera's up direction (unit vector).
     * @return Up basis vector, perpendicular to forward and right.
     */
    [[nodiscard]] math::Vec3 getUp() const;

    /**
     * @brief Returns the current yaw angle in degrees.
     * @return Yaw about the up axis.
     */
    [[nodiscard]] float getYaw() const { return m_Yaw; }

    /**
     * @brief Returns the current pitch angle in degrees.
     * @return Elevation above the horizontal plane.
     */
    [[nodiscard]] float getPitch() const { return m_Pitch; }

    /**
     * @brief Returns the current roll angle in degrees.
     * @return Roll about the forward axis.
     */
    [[nodiscard]] float getRoll() const { return m_Roll; }

    /**
     * @brief Returns which projection type is currently active.
     * @return ProjectionType::Perspective or ProjectionType::Orthographic.
     */
    [[nodiscard]] ProjectionType getProjectionType() const { return m_Type; }

    /**
     * @brief Returns the (cached) view matrix, rebuilding it if the pose changed.
     * @return Reference to the current view matrix.
     */
    [[nodiscard]] const math::Mat4& getView() const;

    /**
     * @brief Returns the (cached) projection matrix, rebuilding it if its parameters changed.
     * @return Reference to the current projection matrix.
     */
    [[nodiscard]] const math::Mat4& getProjection() const;

    /**
     * @brief Returns the combined projection-times-view matrix for shaders.
     * @return Projection * View (transforms world space to clip space).
     */
    [[nodiscard]] math::Mat4 getViewProjection() const { return getProjection() * getView(); }

  private:
    /**
     * @brief Bundle of the three orthonormal camera axes.
     */
    struct Basis
    {
        math::Vec3 forward, right,
            up; /**< Camera basis vectors (unit length, mutually perpendicular). */
    };

    /**
     * @brief Computes the current camera basis from yaw/pitch/roll and the frame.
     * @return Basis whose forward points along the view direction.
     */
    [[nodiscard]] Basis basis() const;

    // Pose (angles in degrees, yaw/pitch are measured in a "Y-up" space and
    // rotated into the user's up convention by m_Frame)
    math::Vec3 m_WorldUp; /**< Reference up direction supplied at construction (normalized). */
    glm::quat m_Frame{
        1.f, 0.f, 0.f,
        0.f}; /**< Rotation mapping +Y onto m_WorldUp; lets yaw/pitch be defined in a Y-up space. */
    math::Vec3 m_Position{0.f, 0.f, 3.f}; /**< World-space position (starts 3 units down +Z). */
    float m_Yaw{-90.f}, m_Pitch{0.f},
        m_Roll{0.f}; /**< Orientation angles in degrees (yaw -90 looks along -Z). */

    // Projection
    ProjectionType m_Type{ProjectionType::Perspective}; /**< Active projection kind. */
    float m_Fov{60.f}, m_Aspect{16.f / 9.f}, m_Near{0.1f},
        m_Far{100.f}; /**< Perspective parameters (FOV in degrees). */
    float m_Left{-1.f}, m_Right{1.f}, m_Bottom{-1.f},
        m_Top{1.f}; /**< Orthographic frustum bounds. */

    // Cached matrices
    mutable math::Mat4 m_View{1.f}, m_Projection{1.f}; /**< Lazily rebuilt cached matrices. */
    mutable bool m_ViewDirty{true},
        m_ProjDirty{true}; /**< Dirty flags forcing a matrix rebuild on next query. */
};
} // namespace sog::gfx
