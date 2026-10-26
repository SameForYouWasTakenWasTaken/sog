/**
 * @file FrameBufferTests.cpp
 * @brief Exercises gfx::FrameBuffer: FBO lifecycle, texture/renderbuffer attachment, completeness
 * reporting, release and move semantics.
 */

#include <TestSupport.hpp>

#include <type_traits>
#include <utility>

using sog::FrameBuffer;
using sog::RenderBuffer;
using sog::Texture2D;
using sog::gfx::FramebufferAttachment;

// ======== Type properties ======== //

TEST(FrameBuffer, IsMoveOnly)
{
    static_assert(!std::is_copy_constructible_v<FrameBuffer>, "FrameBuffer must not be copyable");
    static_assert(!std::is_copy_assignable_v<FrameBuffer>,
                  "FrameBuffer must not be copy assignable");
    static_assert(std::is_nothrow_move_constructible_v<FrameBuffer>,
                  "FrameBuffer must be nothrow move constructible");
    static_assert(std::is_nothrow_move_assignable_v<FrameBuffer>,
                  "FrameBuffer must be nothrow move assignable");
    SUCCEED();
}

// ======== Construction ======== //

TEST(FrameBuffer, ConstructionCreatesTheFramebuffer)
{
    const FrameBuffer frameBuffer;

    EXPECT_TRUE(frameBuffer.valid());
    EXPECT_NE(frameBuffer.id(), 0u);
    EXPECT_TRUE(static_cast<bool>(frameBuffer));
    expectNoGLError();
}

// ======== Attachments and completeness ======== //

TEST(FrameBuffer, ColorTexturePlusDepthRenderbufferIsComplete)
{
    FrameBuffer frameBuffer;
    const Texture2D color = Texture2D::empty(8, 8);
    const RenderBuffer depth{GL_DEPTH24_STENCIL8, 8, 8};

    frameBuffer.attachTexture(FramebufferAttachment::Color0, color.getHandle());
    frameBuffer.attachRenderbuffer(FramebufferAttachment::DepthStencil, depth);

    EXPECT_TRUE(frameBuffer.isComplete());
    expectNoGLError();
}

TEST(FrameBuffer, ColorTextureAttachedToTheDepthSlotIsIncomplete)
{
    // A plain RGBA8 image can not serve as depth storage, so the completeness check must fail.
    FrameBuffer frameBuffer;
    const Texture2D notADepthImage = Texture2D::empty(8, 8);

    frameBuffer.attachTexture(FramebufferAttachment::Depth, notADepthImage.getHandle());

    EXPECT_FALSE(frameBuffer.isComplete());
    drainGLErrors();
}

TEST(FrameBuffer, OffscreenTargetFromTestSupportIsComplete)
{
    const auto target = sogtest::OffscreenTarget::create(16, 16);

    EXPECT_TRUE(target.frameBuffer.isComplete());
    EXPECT_TRUE(target.frameBuffer.valid());
}

TEST(FrameBuffer, AttachmentsWorkWhileTheFramebufferIsUnbound)
{
    // attach* uses DSA: no bind should be required first (and none may leave an error behind).
    FrameBuffer frameBuffer;
    const Texture2D color = Texture2D::empty(4, 4);

    frameBuffer.attachTexture(FramebufferAttachment::Color0, color.getHandle());
    frameBuffer.attachTexture(GL_COLOR_ATTACHMENT1, color.getHandle());

    expectNoGLError();
}

// ======== bind() / unbind() ======== //

TEST(FrameBuffer, BindRedirectsDrawingToTheFramebuffer)
{
    FrameBuffer frameBuffer;

    frameBuffer.bind();

    GLint bound = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &bound);
    EXPECT_EQ(static_cast<GLuint>(bound), frameBuffer.id());

    FrameBuffer::unbind();
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &bound);
    EXPECT_EQ(bound, 0);
    expectNoGLError();
}

// ======== release() ======== //

TEST(FrameBuffer, ReleaseDropsTheFramebuffer)
{
    FrameBuffer frameBuffer;
    frameBuffer.release();

    EXPECT_FALSE(frameBuffer.valid());
    EXPECT_EQ(frameBuffer.id(), 0u);
    expectNoGLError();
}

// ======== Move semantics ======== //

TEST(FrameBuffer, MoveConstructorTransfersTheFramebuffer)
{
    FrameBuffer source;
    const GLuint sourceId = source.id();

    FrameBuffer target{std::move(source)};

    EXPECT_EQ(target.id(), sourceId);
    EXPECT_EQ(source.id(), 0u) << "moving must zero the source handle";
    EXPECT_FALSE(source.valid());
}

TEST(FrameBuffer, MoveAssignmentReleasesTheTargetsOldFramebuffer)
{
    FrameBuffer source;
    const GLuint sourceId = source.id();

    FrameBuffer target;
    const GLuint replacedId = target.id();
    ASSERT_NE(replacedId, sourceId);

    target = std::move(source);

    EXPECT_EQ(target.id(), sourceId);
    EXPECT_EQ(source.id(), 0u);
    expectNoGLError();
}

TEST(FrameBuffer, SelfMoveAssignmentIsSafe)
{
    FrameBuffer frameBuffer;
    const GLuint id = frameBuffer.id();

    frameBuffer = std::move(frameBuffer);

    EXPECT_EQ(frameBuffer.id(), id);
    EXPECT_TRUE(frameBuffer.valid());
}
