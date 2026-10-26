/**
 * @file GLHandleTests.cpp
 * @brief Exercises gfx::GLHandle: the typed, move-only holder for a raw OpenGL object name.
 *
 * GLHandle owns nothing but a GLuint name; these tests pin down its move semantics (moving zeroes
 * the source), its validity reporting for the reserved 0 name, and the type-safety that keeps two
 * handles wrapping different GL object kinds from being interchangeable.
 */

#include <TestSupport.hpp>

#include <type_traits>
#include <utility>

using sog::gfx::GLHandle;
using sog::gfx::FrameBufferHandle;
using sog::gfx::ShaderHandle;
using sog::gfx::TextureHandle;

namespace
{
/// Phantom tag for building an arbitrary handle in these tests.
struct DummyTag
{
};
} // namespace

/// Arbitrary handle instantiation used to test GLHandle's generic behavior.
using DummyHandle = GLHandle<DummyTag>;

// ======== Compile-time type properties ======== //
// If any of these assertions ever stops compiling, the move-only or type-safety guarantee has been
// broken.

static_assert(std::is_move_constructible_v<DummyHandle>,
              "GLHandle must be nothrow move constructible");
static_assert(std::is_nothrow_move_assignable_v<DummyHandle>,
              "GLHandle must be nothrow move assignable");
static_assert(!std::is_copy_constructible_v<DummyHandle>,
              "GLHandle must not be copy constructible (moves only)");
static_assert(!std::is_copy_assignable_v<DummyHandle>,
              "GLHandle must not be copy assignable (moves only)");
static_assert(!std::is_constructible_v<TextureHandle, FrameBufferHandle&&>,
              "TextureHandle must not be constructible from a FrameBufferHandle");
static_assert(!std::is_constructible_v<ShaderHandle, TextureHandle&&>,
              "ShaderHandle must not be constructible from a TextureHandle");
static_assert(!std::is_assignable_v<TextureHandle&, FrameBufferHandle&&>,
              "TextureHandle must not be assignable from a FrameBufferHandle");

// ======== Runtime behavior ======== //

TEST(GLHandle, DefaultConstructedHandleIsInvalid)
{
    DummyHandle handle;
    EXPECT_FALSE(handle.valid());
    EXPECT_EQ(handle.get(), 0u);
    EXPECT_FALSE(static_cast<bool>(handle));
}

// ======== Move semantics ======== //

TEST(GLHandle, MoveConstructorTransfersAndZeroesSource)
{
    DummyHandle source;
    source.raw() = 42;

    DummyHandle target{std::move(source)};

    EXPECT_TRUE(target.valid());
    EXPECT_EQ(target.get(), 42u);
    EXPECT_FALSE(source.valid()) << "moving must zero the source handle";
    EXPECT_EQ(source.get(), 0u);
}

TEST(GLHandle, MoveAssignmentTransfersAndZeroesSource)
{
    DummyHandle source;
    source.raw() = 7;

    DummyHandle target;
    target = std::move(source);

    EXPECT_EQ(target.get(), 7u);
    EXPECT_FALSE(source.valid());
}

TEST(GLHandle, SelfMoveAssignmentIsSafe)
{
    DummyHandle handle;
    handle.raw() = 5;

    handle = std::move(handle);

    EXPECT_TRUE(handle.valid());
    EXPECT_EQ(handle.get(), 5u);
}

// ======== raw() and conversion ======== //

TEST(GLHandle, RawExposesMutableName)
{
    DummyHandle handle;
    handle.raw() = 99;

    EXPECT_EQ(handle.get(), 99u);
    EXPECT_TRUE(handle.valid());
}

TEST(GLHandle, ConvertsToUnderlyingHandleType)
{
    DummyHandle handle;
    handle.raw() = 11;

    const sog::gfx::gl_handle raw = handle;
    EXPECT_EQ(raw, 11u);
}

// ======== Type safety ======== //
// The compile-time assertions above pin down that handles wrapping different GL object kinds are
// unrelated; there is no runtime behavior left to exercise here.
