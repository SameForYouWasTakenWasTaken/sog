/**
 * @file RenderBufferTests.cpp
 * @brief Exercises gfx::RenderBuffer: offscreen renderbuffer allocation with a given format/size,
 * release, re-allocation and move semantics.
 */

#include <TestSupport.hpp>

#include <type_traits>
#include <utility>

using sog::RenderBuffer;

namespace
{
/// Reads a renderbuffer allocation parameter (must be called with the renderbuffer bound).
GLint renderbufferParameter(GLenum parameter)
{
    GLint value = 0;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, parameter, &value);
    return value;
}
} // namespace

// ======== Type properties ======== //

TEST(RenderBuffer, IsMoveOnly)
{
    static_assert(!std::is_copy_constructible_v<RenderBuffer>, "RenderBuffer must not be copyable");
    static_assert(!std::is_copy_assignable_v<RenderBuffer>,
                  "RenderBuffer must not be copy assignable");
    static_assert(std::is_nothrow_move_constructible_v<RenderBuffer>,
                  "RenderBuffer must be nothrow move constructible");
    static_assert(std::is_nothrow_move_assignable_v<RenderBuffer>,
                  "RenderBuffer must be nothrow move assignable");
    SUCCEED();
}

// ======== Construction ======== //

TEST(RenderBuffer, DefaultConstructedIsEmpty)
{
    const RenderBuffer renderBuffer;

    EXPECT_FALSE(renderBuffer.valid());
    EXPECT_FALSE(static_cast<bool>(renderBuffer));
    EXPECT_EQ(renderBuffer.id(), 0u);
    EXPECT_EQ(renderBuffer.width(), 0);
    EXPECT_EQ(renderBuffer.height(), 0);
    EXPECT_EQ(renderBuffer.internalFormat(), 0u);
}

TEST(RenderBuffer, ConstructionAllocatesStorageWithTheRequestedShape)
{
    const RenderBuffer renderBuffer{GL_DEPTH24_STENCIL8, 32, 16};

    EXPECT_TRUE(renderBuffer.valid());
    EXPECT_NE(renderBuffer.id(), 0u);
    EXPECT_EQ(renderBuffer.width(), 32);
    EXPECT_EQ(renderBuffer.height(), 16);
    EXPECT_EQ(renderBuffer.internalFormat(), static_cast<GLenum>(GL_DEPTH24_STENCIL8));
    expectNoGLError();
}

TEST(RenderBuffer, AllocatedStorageMatchesTheRecordedShape)
{
    const RenderBuffer renderBuffer{GL_DEPTH24_STENCIL8, 32, 16};

    renderBuffer.bind();
    EXPECT_EQ(renderbufferParameter(GL_RENDERBUFFER_WIDTH), 32);
    EXPECT_EQ(renderbufferParameter(GL_RENDERBUFFER_HEIGHT), 16);
    EXPECT_EQ(renderbufferParameter(GL_RENDERBUFFER_INTERNAL_FORMAT), GL_DEPTH24_STENCIL8);

    RenderBuffer::unbind();
    expectNoGLError();
}

// ======== create() / release() ======== //

TEST(RenderBuffer, CreateAfterReleaseReallocates)
{
    RenderBuffer renderBuffer{GL_DEPTH24_STENCIL8, 8, 8};
    renderBuffer.release();
    ASSERT_FALSE(renderBuffer.valid());

    renderBuffer.create(GL_RGBA8, 4, 4);

    EXPECT_TRUE(renderBuffer.valid());
    EXPECT_EQ(renderBuffer.width(), 4);
    EXPECT_EQ(renderBuffer.height(), 4);
    EXPECT_EQ(renderBuffer.internalFormat(), static_cast<GLenum>(GL_RGBA8));
    expectNoGLError();
}

TEST(RenderBuffer, ReleaseResetsShapeAndHandle)
{
    RenderBuffer renderBuffer{GL_DEPTH24_STENCIL8, 8, 8};
    renderBuffer.release();

    EXPECT_FALSE(renderBuffer.valid());
    EXPECT_EQ(renderBuffer.id(), 0u);
    EXPECT_EQ(renderBuffer.width(), 0);
    EXPECT_EQ(renderBuffer.height(), 0);
    EXPECT_EQ(renderBuffer.internalFormat(), 0u);
}

// ======== Move semantics ======== //

TEST(RenderBuffer, MoveConstructorTransfersStorageAndShape)
{
    RenderBuffer source{GL_DEPTH24_STENCIL8, 12, 24};
    const GLuint sourceId = source.id();

    RenderBuffer target{std::move(source)};

    EXPECT_EQ(target.id(), sourceId);
    EXPECT_EQ(target.width(), 12);
    EXPECT_EQ(target.height(), 24);

    EXPECT_EQ(source.id(), 0u);
    EXPECT_EQ(source.width(), 0) << "moving must reset the source's recorded shape";
    EXPECT_EQ(source.height(), 0);
    EXPECT_EQ(source.internalFormat(), 0u);
}

TEST(RenderBuffer, MoveAssignmentReleasesTheTargetsOldStorage)
{
    RenderBuffer source{GL_DEPTH24_STENCIL8, 12, 24};
    const GLuint sourceId = source.id();

    RenderBuffer target{GL_RGBA8, 64, 64};
    ASSERT_NE(target.id(), sourceId);

    target = std::move(source);

    EXPECT_EQ(target.id(), sourceId);
    EXPECT_EQ(target.width(), 12);
    EXPECT_EQ(source.id(), 0u);
    expectNoGLError();
}

TEST(RenderBuffer, SelfMoveAssignmentIsSafe)
{
    RenderBuffer renderBuffer{GL_DEPTH24_STENCIL8, 8, 8};
    const GLuint id = renderBuffer.id();

    renderBuffer = std::move(renderBuffer);

    EXPECT_EQ(renderBuffer.id(), id);
    EXPECT_EQ(renderBuffer.width(), 8);
}
