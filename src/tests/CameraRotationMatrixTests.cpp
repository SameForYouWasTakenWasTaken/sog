/**
 * @file CameraRotationMatrixTests.cpp
 * @brief Verifies gfx::Camera's rotation behavior and the getRotationMatrix() helper.
 *
 * The user reported that building a model's orientation by hand-applying glm::rotate() for
 * yaw/pitch in a fixed Euler order "appears inaccurate" compared to the camera's own basis
 * (right/up/-forward). These tests pin down exactly why: the camera derives its orientation from
 * its forward/right/up basis (which also folds in the world-up frame and roll), whereas a naive
 * Euler composition assumes a specific axis order that does not match the camera's construction.
 * getRotationMatrix() is the correct-by-construction alternative.
 */

#include <TestSupport.hpp>

#include <glm/gtc/matrix_transform.hpp>

using sog::Camera;
using sog::math::Mat4;
using sog::math::Vec3;
using sog::math::Vec4;

namespace
{
/// Tolerance for float comparisons of basis vectors and rotation matrices.
constexpr float TOLERANCE = 1e-4f;

/**
 * @brief Builds the camera-style orientation matrix: columns are right, up and -forward.
 * @param camera Whose basis to sample.
 * @return The camera's world orientation (maps camera-local axes to world space).
 */
Mat4 basisColumns(const Camera& camera)
{
    Mat4 rotation{1.f};
    rotation[0] = Vec4{camera.getRight(), 0.f};
    rotation[1] = Vec4{camera.getUp(), 0.f};
    rotation[2] = Vec4{-camera.getForward(), 0.f};
    return rotation;
}

/**
 * @brief Reproduces the user's naive Euler composition: rotate about +Y by yaw, then about +X by
 * -pitch.
 * @param yaw Yaw angle in degrees.
 * @param pitch Pitch angle in degrees.
 * @return The orientation a fixed-order Euler composition produces.
 */
Mat4 naiveEulerComposition(float yaw, float pitch)
{
    Mat4 rotation{1.f};
    rotation = glm::rotate(rotation, glm::radians(yaw), Vec3{0.f, 1.f, 0.f});
    rotation = glm::rotate(rotation, glm::radians(-pitch), Vec3{1.f, 0.f, 0.f});
    return rotation;
}
} // namespace

TEST(CameraRotationMatrix, IdentityAtDefaultPose)
{
    Camera camera;

    // At the default pose (yaw -90, pitch 0, roll 0) the camera looks straight down -Z with
    // right = +X and up = +Y, so its orientation is the identity.
    expectMatNear(camera.getRotationMatrix(), Mat4{1.f}, TOLERANCE);
}

TEST(CameraRotationMatrix, MatchesBasisColumns)
{
    Camera camera;
    camera.setRotation(-42.f, 25.f, 10.f);

    expectMatNear(camera.getRotationMatrix(), basisColumns(camera), TOLERANCE);
}

TEST(CameraRotationMatrix, IsPureRotation)
{
    Camera camera;
    camera.setRotation(33.f, -48.f, 17.f);

    const Mat4 rotation = camera.getRotationMatrix();

    // A pure rotation has orthonormal columns and no translation.
    expectVecNear(Vec3{rotation[3]}, Vec3{0.f, 0.f, 0.f}, TOLERANCE);

    const Vec3 column0{rotation[0]};
    const Vec3 column1{rotation[1]};
    const Vec3 column2{rotation[2]};

    EXPECT_NEAR(glm::length(column0), 1.f, TOLERANCE);
    EXPECT_NEAR(glm::length(column1), 1.f, TOLERANCE);
    EXPECT_NEAR(glm::length(column2), 1.f, TOLERANCE);
    EXPECT_NEAR(glm::dot(column0, column1), 0.f, TOLERANCE);
    EXPECT_NEAR(glm::dot(column0, column2), 0.f, TOLERANCE);
    EXPECT_NEAR(glm::dot(column1, column2), 0.f, TOLERANCE);

    // Right-handed: the columns' cross product must equal the third column.
    expectVecNear(glm::cross(column0, column1), column2, TOLERANCE);
}

TEST(CameraRotationMatrix, ForwardColumnIsNegatedForward)
{
    Camera camera;
    camera.setRotation(-110.f, -34.f, 0.f);

    const Vec3 column2{camera.getRotationMatrix()[2]};
    expectVecNear(column2, Vec3{-camera.getForward()}, TOLERANCE);
}

TEST(CameraRotationMatrix, RollRotatesRightAndUpAboutForward)
{
    Camera camera;
    camera.setRotation(-90.f, 0.f, 0.f);
    const Vec3 rightWithoutRoll = camera.getRight();
    const Vec3 upWithoutRoll = camera.getUp();

    // Positive roll is a right-handed rotation about the forward axis, so +90 degrees sends
    // right onto -up and up onto +right...
    camera.setRotation(-90.f, 0.f, 90.f);
    expectVecNear(camera.getRight(), Vec3{-upWithoutRoll}, TOLERANCE);
    expectVecNear(camera.getUp(), Vec3{rightWithoutRoll}, TOLERANCE);
    EXPECT_GT(glm::length(camera.getRight() - rightWithoutRoll), 1.f);

    // ...while -90 degrees sends right onto +up (the intuitive "roll right onto up").
    camera.setRotation(-90.f, 0.f, -90.f);
    expectVecNear(camera.getRight(), Vec3{upWithoutRoll}, TOLERANCE);
    expectVecNear(camera.getUp(), Vec3{-rightWithoutRoll}, TOLERANCE);
}

TEST(CameraRotationMatrix, WorldUpFrameIsApplied)
{
    // A Z-up world: yaw/pitch are still defined in a Y-up space and then mapped through the frame.
    Camera camera{Vec3{0.f, 0.f, 1.f}};
    camera.setRotation(-90.f, 0.f, 0.f);

    // In the Y-up local space forward is (0, 0, -1); the frame rotating +Y onto +Z maps that onto
    // +Y, so the camera looks along the world's "horizontal" forward for a Z-up world.
    const Vec3 forward = camera.getForward();
    EXPECT_NEAR(glm::length(forward), 1.f, TOLERANCE);
    EXPECT_GT(forward.y, 0.9f);

    // The up vector must be pulled toward the configured world up (+Z).
    EXPECT_GT(camera.getUp().z, 0.f);
}

TEST(CameraRotationMatrix, WorldUpFrameKeepsRotationMatrixValid)
{
    Camera camera{Vec3{0.f, 0.f, 1.f}};
    camera.setRotation(-25.f, 40.f, 15.f);

    const Mat4 rotation = camera.getRotationMatrix();
    const Vec3 column0{rotation[0]};
    const Vec3 column1{rotation[1]};
    const Vec3 column2{rotation[2]};

    expectVecNear(glm::cross(column0, column1), column2, TOLERANCE);
    EXPECT_NEAR(glm::length(column0), 1.f, TOLERANCE);
}

TEST(CameraRotationMatrix, RotatesCameraLocalForwardToWorldForward)
{
    Camera camera;
    camera.setRotation(-90.f, 0.f, 0.f);

    // Camera-local forward is (0, 0, -1); the orientation matrix must map it to the world forward.
    const Mat4 rotation = camera.getRotationMatrix();
    const Vec3 rotated{rotation * Vec4{0.f, 0.f, -1.f, 0.f}};

    expectVecNear(rotated, camera.getForward(), TOLERANCE);
}

TEST(CameraRotationMatrix, RotatesCameraLocalRightToWorldRight)
{
    Camera camera;
    camera.setRotation(-135.f, 20.f, 5.f);

    const Mat4 rotation = camera.getRotationMatrix();
    const Vec3 rotated{rotation * Vec4{1.f, 0.f, 0.f, 0.f}};

    expectVecNear(rotated, camera.getRight(), TOLERANCE);
}

TEST(CameraRotationMatrix, ConsistentWithViewRotation)
{
    Camera camera;
    camera.setPosition({3.f, -2.f, 7.f});
    camera.setRotation(-65.f, 18.f, 12.f);

    // The view matrix's rotation is the inverse (transpose, being orthonormal) of the world
    // orientation; its translation is -rotation^T * position.
    const Mat4 view = camera.getView();
    const Mat4 rotation = camera.getRotationMatrix();
    const Mat4 expectedViewRotation = glm::transpose(rotation);

    for (int column = 0; column < 3; ++column)
        for (int row = 0; row < 3; ++row)
            EXPECT_NEAR(view[column][row], expectedViewRotation[column][row], TOLERANCE)
                << "view rotation mismatch at column " << column << ", row " << row;
}

TEST(CameraRotationMatrix, DiffersFromNaiveEulerAtNonDefaultPose)
{
    // This is the crux of the user's report: at a non-default pose the camera's orientation is NOT
    // the same as a fixed-order (Y-then-X) Euler composition of the same yaw/pitch numbers.
    Camera camera;
    camera.setRotation(-90.f, 30.f, 0.f);

    const Mat4 fromCamera = camera.getRotationMatrix();
    const Mat4 fromNaiveEuler = naiveEulerComposition(-90.f, 30.f);

    // They agree that both are rotations, but disagree on the resulting orientation: the camera's
    // basis folds in its own axis conventions (and the -90 yaw reference), so the two matrices
    // differ. Callers wanting the camera's orientation must use getRotationMatrix(), not a hand
    // -rolled Euler composition.
    bool differs = false;
    for (int column = 0; column < 4 && !differs; ++column)
        for (int row = 0; row < 4 && !differs; ++row)
            differs = std::abs(fromCamera[column][row] - fromNaiveEuler[column][row]) > TOLERANCE;

    EXPECT_TRUE(differs)
        << "getRotationMatrix() should not coincide with a fixed-order Euler composition";
}

TEST(CameraRotationMatrix, ZeroYawLooksAlongPlusX)
{
    // The camera measures yaw from +X: with yaw 0 and pitch 0 the basis is forward = +X,
    // right = +Z and up = +Y (the identity orientation is the *default* yaw of -90, covered
    // above). This documents the axis conventions so a caller can align a model to the camera.
    Camera camera;
    camera.setRotation(0.f, 0.f, 0.f);

    expectVecNear(camera.getForward(), Vec3{1.f, 0.f, 0.f}, TOLERANCE);
    expectVecNear(camera.getRight(), Vec3{0.f, 0.f, 1.f}, TOLERANCE);
    expectVecNear(camera.getUp(), Vec3{0.f, 1.f, 0.f}, TOLERANCE);

    // Orientation = columns right | up | -forward for this pose.
    Mat4 expected{1.f};
    expected[0] = Vec4{0.f, 0.f, 1.f, 0.f};
    expected[1] = Vec4{0.f, 1.f, 0.f, 0.f};
    expected[2] = Vec4{-1.f, 0.f, 0.f, 0.f};
    expectMatNear(camera.getRotationMatrix(), expected, TOLERANCE);
}
