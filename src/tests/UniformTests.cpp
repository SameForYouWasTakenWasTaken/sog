/**
 * @file UniformTests.cpp
 * @brief Exercises the sog::gfx::uniform helpers: lookup, typed uploads (verified by reading the
 * values back with glGetUniform*), invalid-location error paths and uniform::runSafe.
 *
 * All tests link the same two-stage program whose fragment shader consumes seven uniforms, so
 * every setter can be exercised against an active uniform rather than one the linker removed.
 */

#include <TestSupport.hpp>

#include <array>
#include <memory>
#include <stdexcept>
#include <string>

using sog::Uniform;
using sog::graphics_error;
using sog::graphics_error_code;

namespace
{
/// Fixture linking PASS_THROUGH_VERT + UNIFORM_FRAG once per test (fresh uniform state each time).
class UniformTest : public sogtest::GLTest
{
  protected:
    void SetUp() override
    {
        sogtest::GLTest::SetUp();

        const sog::VertexShader vertex{sogtest::PASS_THROUGH_VERT};
        const sog::FragmentShader fragment{sogtest::UNIFORM_FRAG};
        m_Program = std::make_unique<sog::ShaderProgram>(vertex, fragment);
    }

    void TearDown() override
    {
        m_Program.reset();
        sogtest::GLTest::TearDown();
    }

    /// Looks up a uniform that must exist in the test shader (fails the test if it does not).
    [[nodiscard]] Uniform mustFind(const std::string& name)
    {
        const auto location = m_Program->getLocation(name);
        EXPECT_TRUE(location.has_value()) << "uniform " << name << " is missing from UNIFORM_FRAG";
        return location.value_or(Uniform{});
    }

    [[nodiscard]] GLuint programId() const { return m_Program->get().get(); }

    /// Reads a float uniform back from the driver.
    [[nodiscard]] float readFloat(const Uniform& uniform) const
    {
        float value = -1.f;
        glGetUniformfv(programId(), uniform.location, &value);
        return value;
    }

    /// Reads an int uniform back from the driver.
    [[nodiscard]] int readInt(const Uniform& uniform) const
    {
        GLint value = -1;
        glGetUniformiv(programId(), uniform.location, &value);
        return value;
    }

    /// Reads a float vector uniform back from the driver (up to 4 components).
    [[nodiscard]] std::array<float, 4> readVec(const Uniform& uniform) const
    {
        std::array<float, 4> values{-1.f, -1.f, -1.f, -1.f};
        glGetUniformfv(programId(), uniform.location, values.data());
        return values;
    }

    /// Reads a 4x4 matrix uniform back from the driver (column-major).
    [[nodiscard]] std::array<float, 16> readMat4(const Uniform& uniform) const
    {
        std::array<float, 16> values{};
        glGetUniformfv(programId(), uniform.location, values.data());
        return values;
    }

    /// Reads a 3x3 matrix uniform back from the driver (column-major, tightly packed).
    [[nodiscard]] std::array<float, 9> readMat3(const Uniform& uniform) const
    {
        // glGetUniform* returns plain (non-block) matrices column-major and packed, so a mat3
        // occupies 9 consecutive floats -- no std140-style row padding.
        std::array<float, 9> values{};
        glGetUniformfv(programId(), uniform.location, values.data());
        return values;
    }

    std::unique_ptr<sog::ShaderProgram> m_Program;
};
} // namespace

// ======== Location lookup ======== //

TEST_F(UniformTest, GetLocationFindsEveryUniformInTestShader)
{
    for (const char* name : {"uColor", "uOffset", "uTint", "uMat3", "uMat4", "uBoost", "uInt"})
    {
        const auto location = sog::uniform::getLocation(*m_Program, name);
        EXPECT_TRUE(location.valid()) << "uniform " << name << " should resolve";
        EXPECT_NE(location.location, -1);
    }
    expectNoGLError();
}

TEST_F(UniformTest, GetLocationIsInvalidForUnknownUniform)
{
    const auto location = sog::uniform::getLocation(*m_Program, "uNoSuchUniform");

    EXPECT_FALSE(location.valid());
    EXPECT_FALSE(static_cast<bool>(location));
    EXPECT_EQ(location.location, -1);
    // A missing uniform must not itself raise a GL error: glGetUniformLocation just returns -1.
    expectNoGLError();
}

// ======== Typed uploads ======== //

TEST_F(UniformTest, SetVec4UploadsAllFourComponents)
{
    sog::uniform::setVec4(*m_Program, mustFind("uColor"), {0.25f, 0.5f, 0.75f, 1.f});

    const auto values = readVec(mustFind("uColor"));
    EXPECT_NEAR(values[0], 0.25f, 1e-6f);
    EXPECT_NEAR(values[1], 0.5f, 1e-6f);
    EXPECT_NEAR(values[2], 0.75f, 1e-6f);
    EXPECT_NEAR(values[3], 1.f, 1e-6f);
    expectNoGLError();
}

TEST_F(UniformTest, SetVec3UploadsAllThreeComponents)
{
    sog::uniform::setVec3(*m_Program, mustFind("uTint"), {0.1f, 0.2f, 0.3f});

    const auto values = readVec(mustFind("uTint"));
    EXPECT_NEAR(values[0], 0.1f, 1e-6f);
    EXPECT_NEAR(values[1], 0.2f, 1e-6f);
    EXPECT_NEAR(values[2], 0.3f, 1e-6f);
    expectNoGLError();
}

TEST_F(UniformTest, SetVec2UploadsBothComponents)
{
    sog::uniform::setVec2(*m_Program, mustFind("uOffset"), {-0.5f, 0.5f});

    const auto values = readVec(mustFind("uOffset"));
    EXPECT_NEAR(values[0], -0.5f, 1e-6f);
    EXPECT_NEAR(values[1], 0.5f, 1e-6f);
    expectNoGLError();
}

TEST_F(UniformTest, SetFloatUploadsScalar)
{
    sog::uniform::setFloat(*m_Program, mustFind("uBoost"), 0.5f);

    EXPECT_NEAR(readFloat(mustFind("uBoost")), 0.5f, 1e-6f);
    expectNoGLError();
}

TEST_F(UniformTest, SetIntUploadsInteger)
{
    sog::uniform::setInt(*m_Program, mustFind("uInt"), 7);

    EXPECT_EQ(readInt(mustFind("uInt")), 7);
    expectNoGLError();
}

TEST_F(UniformTest, SetMat4UploadsMatrix)
{
    const sog::math::Mat4 matrix{
        1.f, 0.f, 0.f, 0.f, // column 0
        0.f, 2.f, 0.f, 0.f, // column 1
        0.f, 0.f, 3.f, 0.f, // column 2
        4.f, 5.f, 6.f, 1.f  // column 3
    };

    sog::uniform::setMat4(*m_Program, mustFind("uMat4"), matrix);

    const auto values = readMat4(mustFind("uMat4"));
    // Column-major: element (row, col) sits at col * 4 + row.
    EXPECT_NEAR(values[0], 1.f, 1e-6f);   // column 0
    EXPECT_NEAR(values[5], 2.f, 1e-6f);   // column 1
    EXPECT_NEAR(values[10], 3.f, 1e-6f);  // column 2
    EXPECT_NEAR(values[12], 4.f, 1e-6f);  // column 3, row 0
    EXPECT_NEAR(values[13], 5.f, 1e-6f);  // column 3, row 1
    EXPECT_NEAR(values[14], 6.f, 1e-6f);  // column 3, row 2
    EXPECT_NEAR(values[15], 1.f, 1e-6f);  // column 3, row 3
    expectNoGLError();
}

TEST_F(UniformTest, SetMat3UploadsMatrix)
{
    const sog::math::Mat3 matrix{2.f, 0.f, 0.f, 0.f, 3.f, 0.f, 0.f, 0.f, 4.f};

    sog::uniform::setMat3(*m_Program, mustFind("uMat3"), matrix);

    const auto values = readMat3(mustFind("uMat3"));
    // Packed column-major: (row, col) sits at col * 3 + row.
    EXPECT_NEAR(values[0], 2.f, 1e-6f);  // column 0, row 0
    EXPECT_NEAR(values[4], 3.f, 1e-6f);  // column 1, row 1
    EXPECT_NEAR(values[8], 4.f, 1e-6f);  // column 2, row 2
    EXPECT_NEAR(values[3], 0.f, 1e-6f);  // column 1, row 0 (off-diagonal)
    EXPECT_NEAR(values[6], 0.f, 1e-6f);  // column 2, row 0 (off-diagonal)
    expectNoGLError();
}

// ======== Error paths ======== //

TEST_F(UniformTest, SettersThrowOnInvalidLocation)
{
    const Uniform missing{-1};

    EXPECT_THROW(sog::uniform::setVec4(*m_Program, missing, {1.f, 2.f, 3.f, 4.f}),
                 graphics_error);
    EXPECT_THROW(sog::uniform::setVec3(*m_Program, missing, {1.f, 2.f, 3.f}), graphics_error);
    EXPECT_THROW(sog::uniform::setVec2(*m_Program, missing, {1.f, 2.f}), graphics_error);
    EXPECT_THROW(sog::uniform::setFloat(*m_Program, missing, 1.f), graphics_error);
    EXPECT_THROW(sog::uniform::setInt(*m_Program, missing, 1), graphics_error);
    EXPECT_THROW(sog::uniform::setMat3(*m_Program, missing, sog::math::Mat3{1.f}), graphics_error);
    EXPECT_THROW(sog::uniform::setMat4(*m_Program, missing, sog::math::Mat4{1.f}), graphics_error);
    drainGLErrors();
}

TEST_F(UniformTest, InvalidLocationErrorCarriesUniformLocationInvalidCode)
{
    try
    {
        sog::uniform::setFloat(*m_Program, Uniform{-1}, 1.f);
        FAIL() << "uploading to an invalid uniform must throw";
    }
    catch (const graphics_error& error)
    {
        EXPECT_EQ(error.code(), graphics_error_code::uniform_location_invalid);
    }
}

// ======== Uniform value type ======== //

TEST(Uniform, DefaultConstructedIsInvalid)
{
    const Uniform uniform;

    EXPECT_EQ(uniform.location, -1);
    EXPECT_FALSE(uniform.valid());
    EXPECT_FALSE(static_cast<bool>(uniform));
}

TEST(Uniform, ConstructedWithLocationIsValid)
{
    const Uniform uniform{3};

    EXPECT_EQ(uniform.location, 3);
    EXPECT_TRUE(uniform.valid());
    EXPECT_TRUE(static_cast<bool>(uniform));
    EXPECT_EQ(static_cast<GLint>(uniform), 3);
}

TEST(Uniform, LocationZeroIsValid)
{
    // GL location 0 is perfectly valid; only -1 means "missing".
    const Uniform uniform{0};

    EXPECT_TRUE(uniform.valid());
}

// ======== runSafe ======== //

TEST(UniformRunSafe, RunsEveryCallbackDespiteExceptions)
{
    int calls = 0;

    sog::uniform::runSafe(
        [&]
        {
            ++calls;
            throw std::runtime_error{"first callback fails"};
        },
        [&] { ++calls; },
        [&] { ++calls; });

    EXPECT_EQ(calls, 3) << "an exception in one callback must not skip the others";
}

TEST(UniformRunSafe, SwallowsExceptions)
{
    EXPECT_NO_THROW(sog::uniform::runSafe([] { throw std::runtime_error{"boom"}; }));
}

TEST(UniformRunSafe, WithNoCallbacksIsANoOp)
{
    EXPECT_NO_THROW(sog::uniform::runSafe());
    SUCCEED();
}
