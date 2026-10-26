/**
 * @file ShaderProgramTests.cpp
 * @brief Exercises gfx::ShaderProgram: program creation, attaching + linking two shader stages,
 * use() state changes, uniform-location lookup/caching and link-failure reporting.
 */

#include <TestSupport.hpp>

#include <string_view>
#include <type_traits>

using sog::FragmentShader;
using sog::ShaderProgram;
using sog::VertexShader;
using sog::graphics_error;
using sog::graphics_error_code;

namespace
{
/// Vertex stage writing a vec4 varying...
constexpr std::string_view LINK_FAIL_VERT = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
out vec4 vColor;
void main()
{
    gl_Position = vec4(aPos, 1.0);
    vColor = vec4(1.0);
}
)";

/// ... and a fragment stage reading the same name as a vec3: legal per stage, invalid at link.
constexpr std::string_view LINK_FAIL_FRAG = R"(
#version 460 core
in vec3 vColor;
out vec4 FragColor;
void main() { FragColor = vec4(vColor, 1.0); }
)";

/// Returns the raw GL name of a program for state queries.
GLuint rawId(const ShaderProgram& program) { return program.get().get(); }
} // namespace

// ======== Type properties ======== //

TEST(ShaderProgram, IsMoveOnly)
{
    static_assert(!std::is_copy_constructible_v<ShaderProgram>,
                  "ShaderProgram must not be copy constructible");
    static_assert(std::is_move_constructible_v<ShaderProgram>,
                  "ShaderProgram must be move constructible");
    SUCCEED();
}

// ======== Construction / linking ======== //

TEST(ShaderProgram, DefaultConstructionCreatesProgramObject)
{
    const ShaderProgram program;

    EXPECT_NE(rawId(program), 0u);
}

TEST(ShaderProgram, ConstructingWithTwoShadersLinksTheProgram)
{
    const VertexShader vertex{sogtest::PASS_THROUGH_VERT};
    const FragmentShader fragment{sogtest::SOLID_RED_FRAG};

    const ShaderProgram program{vertex, fragment};

    EXPECT_NE(rawId(program), 0u);

    GLint linked = GL_FALSE;
    glGetProgramiv(rawId(program), GL_LINK_STATUS, &linked);
    EXPECT_EQ(linked, GL_TRUE);
    expectNoGLError();
}

TEST(ShaderProgram, InitializeLinksAnExistingProgram)
{
    const VertexShader vertex{sogtest::PASS_THROUGH_VERT};
    const FragmentShader fragment{sogtest::SOLID_RED_FRAG};
    ShaderProgram program;

    program.Initialize(vertex, fragment);

    GLint linked = GL_FALSE;
    glGetProgramiv(rawId(program), GL_LINK_STATUS, &linked);
    EXPECT_EQ(linked, GL_TRUE);
    expectNoGLError();
}

// ======== use() ======== //

TEST(ShaderProgram, UseMakesTheProgramCurrent)
{
    const VertexShader vertex{sogtest::PASS_THROUGH_VERT};
    const FragmentShader fragment{sogtest::SOLID_RED_FRAG};
    const ShaderProgram program{vertex, fragment};

    program.use();

    GLint current = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);
    EXPECT_EQ(static_cast<GLuint>(current), rawId(program));
    expectNoGLError();
}

// ======== Link failure ======== //

TEST(ShaderProgram, MismatchedVaryingTypesThrowShaderLinkFailed)
{
    const VertexShader vertex{LINK_FAIL_VERT};
    const FragmentShader fragment{LINK_FAIL_FRAG};

    try
    {
        const ShaderProgram program{vertex, fragment};
        FAIL() << "linking mismatched varyings must throw";
    }
    catch (const graphics_error& error)
    {
        EXPECT_EQ(error.code(), graphics_error_code::shader_link_failed);
        EXPECT_FALSE(error.message().empty()) << "the driver info log must be reported";
    }
}

TEST(ShaderProgram, LinkFailureIsCatchableAsStdException)
{
    const VertexShader vertex{LINK_FAIL_VERT};
    const FragmentShader fragment{LINK_FAIL_FRAG};

    bool caught = false;
    try
    {
        const ShaderProgram program{vertex, fragment};
    }
    catch (const std::exception&)
    {
        caught = true;
    }

    EXPECT_TRUE(caught) << "a failed link must throw something catchable as std::exception";
}

// ======== Uniform lookup ======== //

TEST(ShaderProgram, GetLocationFindsExistingUniforms)
{
    const VertexShader vertex{sogtest::PASS_THROUGH_VERT};
    const FragmentShader fragment{sogtest::UNIFORM_FRAG};
    ShaderProgram program{vertex, fragment};

    for (const char* name : {"uColor", "uOffset", "uTint", "uMat3", "uMat4", "uBoost", "uInt"})
    {
        const auto location = program.getLocation(name);
        ASSERT_TRUE(location.has_value()) << "uniform " << name << " should be found";
        EXPECT_TRUE(location->valid()) << "uniform " << name << " should have a valid location";
    }
    expectNoGLError();
}

TEST(ShaderProgram, GetLocationReturnsNulloptForUnknownUniform)
{
    const VertexShader vertex{sogtest::PASS_THROUGH_VERT};
    const FragmentShader fragment{sogtest::SOLID_RED_FRAG};
    ShaderProgram program{vertex, fragment};

    EXPECT_EQ(program.getLocation("uDefinitelyNotThere"), std::nullopt);
    expectNoGLError();
}

TEST(ShaderProgram, GetLocationIsStableAcrossCalls)
{
    const VertexShader vertex{sogtest::PASS_THROUGH_VERT};
    const FragmentShader fragment{sogtest::UNIFORM_FRAG};
    ShaderProgram program{vertex, fragment};

    const auto first = program.getLocation("uColor");
    const auto second = program.getLocation("uColor");

    ASSERT_TRUE(first.has_value());
    ASSERT_TRUE(second.has_value());
    EXPECT_EQ(first->location, second->location);
}
