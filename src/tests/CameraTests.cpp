/**
 * @file CameraTests.cpp
 * @brief Exercises gfx::Camera pose control, basis vectors, projection selection and the cached
 * view/projection matrices.
 *
 * The rotation-matrix behavior the user specifically wanted verified lives in
 * CameraRotationMatrixTests.cpp; this file covers the rest of the Camera surface.
 */

#include <TestSupport.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

using sog::Camera;
using sog::math::Mat4;
using sog::math::Vec3;
using sog::math::Vec4;

namespace
{
/// Tolerance for float comparisons of camera basis vectors and matrices.
constexpr float TOLERANCE = 1e-4f;
} // namespace

// ======== Defaults ======== //

TEST(Camera, DefaultConstruction)
{
    Camera camera;

    expectVecNear(camera.getPosition(), Vec3{0.f, 0.f, 0.f}, TOLERANCE);
    // The camera's default yaw of -90 with pitch 0 makes forward point along -Z.
    EXPECT_FLOAT_EQ(camera.getYaw(), -90.f);
    EXPECT_FLOAT_EQ(camera.getPitch(), 0.f);
    EXPECT_FLOAT_EQ(camera.getRoll(), 0.f);
    EXPECT_EQ(camera.getProjectionType(), Camera::ProjectionType::Perspective);
    EXPECT_FLOAT_EQ(camera.getForward().z, -1.f);
}

// ======== Pose ======== //

TEST(Camera, SetPositionMovesCamera)
{
    Camera camera;
    camera.setPosition({5.f, 1.f, -2.f});

    expectVecNear(camera.getPosition(), Vec3{5.f, 1.f, -2.f}, TOLERANCE);
}

TEST(Camera, MoveOffsetsPosition)
{
    Camera camera;
    camera.setPosition({1.f, 2.f, 3.f});
    camera.move({0.5f, -1.f, 2.f});

    expectVecNear(camera.getPosition(), Vec3{1.5f, 1.f, 5.f}, TOLERANCE);
}

TEST(Camera, SetRotationUpdatesAngles)
{
    Camera camera;
    camera.setRotation(30.f, -15.f, 5.f);

    EXPECT_FLOAT_EQ(camera.getYaw(), 30.f);
    EXPECT_FLOAT_EQ(camera.getPitch(), -15.f);
    EXPECT_FLOAT_EQ(camera.getRoll(), 5.f);
}

TEST(Camera, SetRotationDefaultsRollToZero)
{
    Camera camera;
    camera.setRotation(10.f, 20.f, 40.f);
    camera.setRotation(45.f, -5.f);

    EXPECT_FLOAT_EQ(camera.getRoll(), 0.f);
}

TEST(Camera, RotateByAccumulates)
{
    Camera camera;
    camera.setRotation(0.f, 0.f, 0.f);
    camera.rotateBy(10.f, 5.f, 3.f);
    camera.rotateBy(-4.f, 2.f, 0.f);

    EXPECT_FLOAT_EQ(camera.getYaw(), 6.f);
    EXPECT_FLOAT_EQ(camera.getPitch(), 7.f);
    EXPECT_FLOAT_EQ(camera.getRoll(), 3.f);
}

TEST(Camera, LookAtAimsForwardAtTarget)
{
    Camera camera;
    camera.setPosition({0.f, 0.f, 5.f});
    camera.lookAt({0.f, 0.f, 5.f}, {0.f, 0.f, -5.f});

    const Vec3 forward = camera.getForward();
    expectVecNear(forward, Vec3{0.f, 0.f, -1.f}, TOLERANCE);
}

TEST(Camera, MoveLocalTranslatesAlongBasis)
{
    Camera camera;
    camera.setPosition({0.f, 0.f, 0.f});
    camera.setRotation(-90.f, 0.f, 0.f);

    // forward is (0, 0, -1) at the default orientation, so moving forward 2 units goes to z=-2.
    camera.moveLocal(0.f, 0.f, 2.f);
    expectVecNear(camera.getPosition(), Vec3{0.f, 0.f, -2.f}, TOLERANCE);
}

// ======== Basis vectors ======== //

TEST(Camera, BasisIsOrthonormal)
{
    Camera camera;
    camera.setRotation(-37.f, 22.f, 11.f);

    const Vec3 forward = camera.getForward();
    const Vec3 right = camera.getRight();
    const Vec3 up = camera.getUp();

    EXPECT_NEAR(glm::length(forward), 1.f, TOLERANCE);
    EXPECT_NEAR(glm::length(right), 1.f, TOLERANCE);
    EXPECT_NEAR(glm::length(up), 1.f, TOLERANCE);

    EXPECT_NEAR(glm::dot(forward, right), 0.f, TOLERANCE);
    EXPECT_NEAR(glm::dot(forward, up), 0.f, TOLERANCE);
    EXPECT_NEAR(glm::dot(right, up), 0.f, TOLERANCE);
}

TEST(Camera, ForwardDependsOnlyOnYawAndPitch)
{
    Camera camera;
    camera.setRotation(-90.f, 0.f, 0.f);
    const Vec3 noRollForward = camera.getForward();

    camera.setRotation(-90.f, 0.f, 45.f);
    const Vec3 withRollForward = camera.getForward();

    // Roll rotates about the forward axis, so it must not change forward itself.
    expectVecNear(withRollForward, noRollForward, TOLERANCE);
}

TEST(Camera, PositivePitchLooksUpward)
{
    Camera camera;
    camera.setRotation(-90.f, 30.f, 0.f);

    EXPECT_GT(camera.getForward().y, 0.f);
}

TEST(Camera, RightPerpendicularToForwardAtPitch)
{
    Camera camera;
    camera.setRotation(-90.f, 60.f, 0.f);

    EXPECT_NEAR(glm::dot(camera.getRight(), camera.getForward()), 0.f, TOLERANCE);
}

// ======== Projection ======== //

TEST(Camera, DefaultProjectionIsPerspective)
{
    Camera camera;

    EXPECT_EQ(camera.getProjectionType(), Camera::ProjectionType::Perspective);
    const Mat4 projection = camera.getProjection();

    // A perspective matrix has a non-trivial [2][3] (the -1 that produces w) and a non-zero
    // [3][2] that depends on the near/far plane.
    EXPECT_FLOAT_EQ(projection[2][3], -1.f);
    EXPECT_NE(projection[3][2], 0.f);
}

TEST(Camera, SetOrthographicSwitchesType)
{
    Camera camera;
    camera.setOrthographic(-10.f, 10.f, -10.f, 10.f, 0.1f, 100.f);

    EXPECT_EQ(camera.getProjectionType(), Camera::ProjectionType::Orthographic);
}

TEST(Camera, PerspectiveProjectionMatchesGlm)
{
    Camera camera;
    camera.setPerspective(45.f, 16.f / 9.f, 0.1f, 100.f);

    const Mat4 expected =
        glm::perspective(glm::radians(45.f), 16.f / 9.f, 0.1f, 100.f);
    expectMatNear(camera.getProjection(), expected, TOLERANCE);
}

TEST(Camera, OrthographicProjectionMatchesGlm)
{
    Camera camera;
    camera.setOrthographic(-4.f, 4.f, -2.f, 2.f, 0.5f, 50.f);

    const Mat4 expected = glm::ortho(-4.f, 4.f, -2.f, 2.f, 0.5f, 50.f);
    expectMatNear(camera.getProjection(), expected, TOLERANCE);
}

TEST(Camera, SetAspectChangesPerspectiveProjection)
{
    Camera camera;
    camera.setPerspective(60.f, 1.f, 0.1f, 100.f);
    const Mat4 before = camera.getProjection();

    camera.setAspect(2.f);
    const Mat4 after = camera.getProjection();

    expectMatNear(after, glm::perspective(glm::radians(60.f), 2.f, 0.1f, 100.f), TOLERANCE);
    EXPECT_NE(before[0][0], after[0][0]);
}

TEST(Camera, SetFovChangesPerspectiveProjection)
{
    Camera camera;
    camera.setPerspective(60.f, 1.f, 0.1f, 100.f);
    camera.setFov(90.f);

    const Mat4 expected = glm::perspective(glm::radians(90.f), 1.f, 0.1f, 100.f);
    expectMatNear(camera.getProjection(), expected, TOLERANCE);
}

TEST(Camera, SetClipPlanesChangesPerspectiveProjection)
{
    Camera camera;
    camera.setPerspective(60.f, 1.f, 0.1f, 100.f);
    camera.setClipPlanes(1.f, 250.f);

    const Mat4 expected = glm::perspective(glm::radians(60.f), 1.f, 1.f, 250.f);
    expectMatNear(camera.getProjection(), expected, TOLERANCE);
}

// ======== View matrix and caching ======== //

TEST(Camera, ViewMatrixTracksPosition)
{
    Camera camera;
    camera.setPosition({0.f, 0.f, 10.f});
    camera.setRotation(-90.f, 0.f, 0.f);

    // glm::lookAt translates by -position in view space (right/up/forward are orthonormal, so
    // the rotation part is orthonormal too and this reproduces the exact expected translation).
    const Mat4 view = camera.getView();
    const Mat4 rotation = camera.getRotationMatrix();
    const Vec4 rotated =
        glm::transpose(rotation) * Vec4{-camera.getPosition().x, -camera.getPosition().y,
                                        -camera.getPosition().z, 1.f};

    expectVecNear(Vec3{view[3]}, Vec3{rotated.x, rotated.y, rotated.z}, TOLERANCE);
}

TEST(Camera, ViewProjectionIsProjectionTimesView)
{
    Camera camera;
    camera.setPosition({1.f, 2.f, 3.f});
    camera.setRotation(-55.f, 12.f, 0.f);

    const Mat4 expected = camera.getProjection() * camera.getView();
    expectMatNear(camera.getViewProjection(), expected, TOLERANCE);
}

TEST(Camera, ViewMatrixIsCachedUntilPoseChanges)
{
    Camera camera;
    const Mat4& first = camera.getView();
    const Mat4* firstAddress = &first;

    // Unchanged pose: getView() must hand back the same cached object.
    const Mat4& second = camera.getView();
    EXPECT_EQ(firstAddress, &second);

    // The cache is a single member rebuilt in place, so invalidation shows up as a content
    // change: after a 15-degree yaw the matrix must no longer equal the default-pose value...
    const Mat4 before = first;
    camera.rotateBy(15.f, 0.f, 0.f);
    const Mat4& third = camera.getView();
    const auto differs = [](const Mat4& a, const Mat4& b) {
        for (int column = 0; column < 4; ++column)
            for (int row = 0; row < 4; ++row)
                if (a[column][row] != b[column][row])
                    return true;
        return false;
    };
    EXPECT_TRUE(differs(before, third)) << "rotating must rebuild the cached view matrix";

    // ... and the rebuilt matrix must match an equivalent camera posed with absolute rotation.
    Camera reference;
    reference.setRotation(-75.f, 0.f, 0.f); // default -90 yaw plus the same +15 increment
    expectMatNear(third, reference.getView(), TOLERANCE);
}

TEST(Camera, ProjectionIsCachedUntilParametersChange)
{
    Camera camera;
    camera.setPerspective(60.f, 16.f / 9.f, 0.1f, 100.f); // known parameters to compare against

    // Unchanged parameters: repeated calls hand back the same cached object.
    const Mat4* firstAddress = &camera.getProjection();
    EXPECT_EQ(firstAddress, &camera.getProjection());

    // A parameter change must rebuild the cache (content differs from the old value)...
    const Mat4 before = camera.getProjection();
    camera.setFov(75.f);
    const Mat4 after = camera.getProjection();
    const auto differs = [](const Mat4& a, const Mat4& b) {
        for (int column = 0; column < 4; ++column)
            for (int row = 0; row < 4; ++row)
                if (a[column][row] != b[column][row])
                    return true;
        return false;
    };
    EXPECT_TRUE(differs(before, after)) << "changing the fov must rebuild the cached projection";

    // ... and the rebuilt matrix is the canonical glm perspective for the new fov.
    expectMatNear(after, glm::perspective(glm::radians(75.f), 16.f / 9.f, 0.1f, 100.f), TOLERANCE);
}

TEST(Camera, MoveMutatorsInvalidateViewCache)
{
    Camera camera;
    const Mat4 before = camera.getView();

    camera.move({1.f, 0.f, 0.f});
    const Mat4 after = camera.getView();

    // Position feeds the view translation, so the rebuilt matrix must differ from the old one.
    const auto differs = [](const Mat4& a, const Mat4& b) {
        for (int column = 0; column < 4; ++column)
            for (int row = 0; row < 4; ++row)
                if (a[column][row] != b[column][row])
                    return true;
        return false;
    };
    EXPECT_TRUE(differs(before, after)) << "moving must rebuild the cached view matrix";
}
