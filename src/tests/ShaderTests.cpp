/**
 * @file ShaderTests.cpp
 * @brief Exercises gfx::Shader: compiling a single GL shader stage from source or file.
 *
 * Covers successful compilation (valid GL object, GL_COMPILE_STATUS set), the graphics_error
 * thrown on a compile failure (code + driver info log) and on a missing file, that the same
 * object can be recompiled from new source, and the move-only ownership model (compile once,
 * transfer the handle, never copy).
 *
 * @note Shader has two converting constructors (source string_view and file path), so these tests
 * always pass an explicitly-typed argument to select the intended overload.
 */

#include <TestSupport.hpp>

#include <filesystem>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

using sog::FragmentShader;
using sog::VertexShader;
using sog::graphics_error;
using sog::graphics_error_code;

namespace
{
/// Minimal, valid vertex shader for compilation tests.
constexpr std::string_view VALID_VERT = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
void main() { gl_Position = vec4(aPos, 1.0); }
)";

/// Minimal, valid fragment shader for compilation tests.
constexpr std::string_view VALID_FRAG = R"(
#version 460 core
out vec4 FragColor;
void main() { FragColor = vec4(1.0, 0.0, 0.0, 1.0); }
)";

/// Deliberately invalid GLSL: not valid syntax, so compilation must fail.
constexpr std::string_view INVALID_SOURCE = R"(
#version 460 core
void main() { this is not valid glsl at all ((( 
)";
} // namespace

// ======== Successful compilation ======== //

TEST(Shader, CompilesVertexShaderFromSource)
{
    const VertexShader shader{VALID_VERT};

    EXPECT_NE(shader.get(), 0u);

    GLint compiled = GL_FALSE;
    glGetShaderiv(shader.get(), GL_COMPILE_STATUS, &compiled);
    EXPECT_EQ(compiled, GL_TRUE);
    expectNoGLError();
}

TEST(Shader, CompilesFragmentShaderFromSource)
{
    const FragmentShader shader{VALID_FRAG};

    EXPECT_NE(shader.get(), 0u);

    GLint compiled = GL_FALSE;
    glGetShaderiv(shader.get(), GL_COMPILE_STATUS, &compiled);
    EXPECT_EQ(compiled, GL_TRUE);
    expectNoGLError();
}

TEST(Shader, DefaultConstructedShaderHasNoObject)
{
    const VertexShader shader;

    EXPECT_EQ(shader.get(), 0u);
}

TEST(Shader, StageIsRecordedAtCreation)
{
    const VertexShader shader{VALID_VERT};

    GLint stage = 0;
    glGetShaderiv(shader.get(), GL_SHADER_TYPE, &stage);
    EXPECT_EQ(static_cast<GLenum>(stage), GL_VERTEX_SHADER);
    expectNoGLError();
}

TEST(Shader, FragmentStageIsRecordedOnFragmentShader)
{
    const FragmentShader shader{VALID_FRAG};

    GLint stage = 0;
    glGetShaderiv(shader.get(), GL_SHADER_TYPE, &stage);
    EXPECT_EQ(static_cast<GLenum>(stage), GL_FRAGMENT_SHADER);
    expectNoGLError();
}

TEST(Shader, RecompilesSameObjectFromNewSource)
{
    VertexShader shader{VALID_VERT};
    const GLuint handleBefore = shader.get();

    shader.createFromSource(VALID_VERT);

    EXPECT_EQ(shader.get(), handleBefore) << "recompiling must reuse the same GL shader object";

    GLint compiled = GL_FALSE;
    glGetShaderiv(shader.get(), GL_COMPILE_STATUS, &compiled);
    EXPECT_EQ(compiled, GL_TRUE);
}

// ======== Failure paths ======== //

TEST(Shader, InvalidSourceThrowsCompilationFailed)
{
    try
    {
        const VertexShader shader{INVALID_SOURCE};
        FAIL() << "compiling invalid GLSL must throw";
    }
    catch (const graphics_error& error)
    {
        EXPECT_EQ(error.code(), graphics_error_code::shader_compilation_failed);
        SUCCEED();
    }
}

TEST(Shader, CompilationErrorCarriesDriverInfoLog)
{
    try
    {
        const VertexShader shader{INVALID_SOURCE};
        FAIL() << "compiling invalid GLSL must throw";
    }
    catch (const graphics_error& error)
    {
        EXPECT_EQ(error.code(), graphics_error_code::shader_compilation_failed);
        EXPECT_FALSE(error.message().empty())
            << "the info log should be carried in the exception message";
        SUCCEED();
    }
}

TEST(Shader, CompilationErrorCanBeCaughtAsStdException)
{
    bool caught = false;
    try
    {
        const FragmentShader shader{INVALID_SOURCE};
    }
    catch (const std::exception& error)
    {
        caught = true;
        EXPECT_NE(error.what(), nullptr);
    }

    EXPECT_TRUE(caught) << "graphics_error must be catchable as std::exception";
}

TEST(Shader, CreateFromSourceOnDefaultConstructedShaderCompilesFresh)
{
    VertexShader shader;
    EXPECT_EQ(shader.get(), 0u);

    shader.createFromSource(VALID_VERT);

    EXPECT_NE(shader.get(), 0u);
    GLint compiled = GL_FALSE;
    glGetShaderiv(shader.get(), GL_COMPILE_STATUS, &compiled);
    EXPECT_EQ(compiled, GL_TRUE);
}

TEST(Shader, MissingFileThrowsFileOpenFailed)
{
    try
    {
        const VertexShader shader{std::filesystem::path{"this/file/does/not/exist.glsl"}};
        FAIL() << "opening a missing shader file must throw";
    }
    catch (const graphics_error& error)
    {
        EXPECT_EQ(error.code(), graphics_error_code::shader_file_open_failed);
        SUCCEED();
    }
}

// ======== createFromFile() ======== //

TEST(Shader, CompilesFromAssetFile)
{
    const std::filesystem::path path{std::string{SOG_TEST_ASSET_DIR} + "/shaders/vertex.glsl"};

    const VertexShader shader{path};

    EXPECT_NE(shader.get(), 0u);

    GLint compiled = GL_FALSE;
    glGetShaderiv(shader.get(), GL_COMPILE_STATUS, &compiled);
    EXPECT_EQ(compiled, GL_TRUE);
    expectNoGLError();
}

// ======== Type properties ======== //

TEST(Shader, ShaderStageAliasesAreDistinct)
{
    static_assert(!std::is_same_v<VertexShader, FragmentShader>,
                  "vertex and fragment shaders must be distinct types");
    SUCCEED();
}

TEST(Shader, IsMoveOnly)
{
    static_assert(!std::is_copy_constructible_v<VertexShader>,
                  "a GL shader name has one owner; copying must be deleted");
    static_assert(!std::is_copy_assignable_v<VertexShader>,
                  "a GL shader name has one owner; copying must be deleted");
    static_assert(std::is_move_constructible_v<VertexShader>,
                  "shaders must be movable so they can be stored in containers");
    static_assert(std::is_move_assignable_v<VertexShader>,
                  "shaders must be move-assignable");
    SUCCEED();
}

// ======== Move semantics ======== //

TEST(Shader, MoveTransfersTheCompiledObject)
{
    VertexShader source{VALID_VERT};
    const GLuint id = source.get();
    ASSERT_NE(id, 0u);

    VertexShader target{std::move(source)};

    EXPECT_EQ(target.get(), id) << "the compiled GL object must transfer to the target";
    EXPECT_EQ(source.get(), 0u) << "the moved-from shader must be empty";
    expectNoGLError("moving must not touch GL state beyond the handle swap");
}

TEST(Shader, MoveAssignmentDeletesTheReplacedObject)
{
    VertexShader target{VALID_VERT};
    VertexShader source{VALID_VERT};
    const GLuint sourceId = source.get();
    ASSERT_NE(sourceId, 0u);

    target = std::move(source);

    EXPECT_EQ(target.get(), sourceId) << "the target must take over the source's object";
    EXPECT_EQ(source.get(), 0u) << "the moved-from shader must be empty";
    expectNoGLError("replacing target's previous object must delete it without a raw GL error");
}
