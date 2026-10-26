/**
 * @file BufferTests.cpp
 * @brief Exercises gfx::backend::GLBuffer: the move-only RAII wrapper over a GL buffer object.
 *
 * These tests create real GL buffers and issue real GL calls (write, bindBinding, bindRange),
 * which requires a current context; they derive from GLTest so the hidden shared context is
 * current before each one runs.
 */

#include <TestSupport.hpp>

#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>

using sog::gfx::backend::GLBuffer;

namespace
{
/// Tolerance for float comparisons.
constexpr float TOLERANCE = 1e-6f;

/// Element type for the test buffers; trivially copyable, like real buffer payloads.
struct SampleElement
{
    float a = 0.f;
    float b = 0.f;
};
} // namespace

// ======== Type properties ======== //

TEST(GLBuffer, IsMoveOnly)
{
    static_assert(!std::is_copy_constructible_v<GLBuffer<SampleElement>>,
                  "GLBuffer must not be copy constructible");
    static_assert(!std::is_copy_assignable_v<GLBuffer<SampleElement>>,
                  "GLBuffer must not be copy assignable");
    static_assert(std::is_nothrow_move_constructible_v<GLBuffer<SampleElement>>,
                  "GLBuffer must be nothrow move constructible");
    static_assert(std::is_nothrow_move_assignable_v<GLBuffer<SampleElement>>,
                  "GLBuffer must be nothrow move assignable");
    SUCCEED();
}

// ======== Construction ======== //

TEST(GLBuffer, CreatingAllocatesStorageOfRequestedSize)
{
    const std::size_t bytes = 4 * sizeof(SampleElement);
    const GLBuffer<SampleElement> buffer{bytes};

    EXPECT_NE(buffer.id(), 0u);
    EXPECT_EQ(buffer.size(), bytes);
}

TEST(GLBuffer, DefaultConstructedBufferIsEmpty)
{
    const GLBuffer<SampleElement> buffer;

    EXPECT_EQ(buffer.id(), 0u);
    EXPECT_EQ(buffer.size(), 0u);
}

// ======== Move semantics ======== //

TEST(GLBuffer, MoveConstructorTransfersStorageAndZeroesSource)
{
    GLBuffer<SampleElement> source{8 * sizeof(SampleElement)};
    const GLuint sourceId = source.id();

    GLBuffer<SampleElement> target{std::move(source)};

    EXPECT_EQ(target.id(), sourceId);
    EXPECT_EQ(target.size(), 8 * sizeof(SampleElement));
    EXPECT_EQ(source.id(), 0u) << "moving must leave the source empty";
    EXPECT_EQ(source.size(), 0u);
}

TEST(GLBuffer, MoveAssignmentReleasesTargetAndTransfersSource)
{
    GLBuffer<SampleElement> source{4 * sizeof(SampleElement)};
    const GLuint sourceId = source.id();

    GLBuffer<SampleElement> target{16 * sizeof(SampleElement)};
    target = std::move(source);

    EXPECT_EQ(target.id(), sourceId);
    EXPECT_EQ(source.id(), 0u);
}

TEST(GLBuffer, SelfMoveAssignmentIsSafe)
{
    GLBuffer<SampleElement> buffer{2 * sizeof(SampleElement)};
    const GLuint id = buffer.id();

    buffer = std::move(buffer);

    EXPECT_NE(buffer.id(), 0u);
    EXPECT_EQ(buffer.id(), id);
}

// ======== write() ======== //

TEST(GLBuffer, WriteStoresDataAtByteOffset)
{
    const std::array<SampleElement, 2> payload{SampleElement{1.f, 2.f}, SampleElement{3.f, 4.f}};
    GLBuffer<SampleElement> buffer{4 * sizeof(SampleElement)};

    buffer.write(0, payload.data(), payload.size() * sizeof(SampleElement));

    EXPECT_NE(buffer.id(), 0u);
    expectNoGLError();
}

TEST(GLBuffer, WriteWithZeroBytesIsANoOp)
{
    GLBuffer<SampleElement> buffer{sizeof(SampleElement)};

    buffer.write(0, nullptr, 0);

    EXPECT_NE(buffer.id(), 0u);
    expectNoGLError();
}

// ======== resize() ======== //

TEST(GLBuffer, ResizeToLargerSizePreservesContents)
{
    const std::array<SampleElement, 2> payload{SampleElement{5.f, 6.f}, SampleElement{7.f, 8.f}};
    GLBuffer<SampleElement> buffer{2 * sizeof(SampleElement)};
    buffer.write(0, payload.data(), payload.size() * sizeof(SampleElement));

    buffer.resize(8 * sizeof(SampleElement), /*keepContents=*/true);

    EXPECT_EQ(buffer.size(), 8 * sizeof(SampleElement));

    std::array<SampleElement, 2> readBack{};
    glGetNamedBufferSubData(buffer.id(), 0, sizeof(readBack), readBack.data());

    EXPECT_NEAR(readBack[0].a, 5.f, TOLERANCE);
    EXPECT_NEAR(readBack[0].b, 6.f, TOLERANCE);
    EXPECT_NEAR(readBack[1].a, 7.f, TOLERANCE);
    EXPECT_NEAR(readBack[1].b, 8.f, TOLERANCE);
}

TEST(GLBuffer, ResizeWithoutKeepingContentsDiscardsData)
{
    const std::array<SampleElement, 1> payload{SampleElement{1.f, 1.f}};
    GLBuffer<SampleElement> buffer{sizeof(SampleElement)};
    buffer.write(0, payload.data(), payload.size() * sizeof(SampleElement));

    buffer.resize(4 * sizeof(SampleElement), /*keepContents=*/false);

    EXPECT_EQ(buffer.size(), 4 * sizeof(SampleElement));
    EXPECT_NE(buffer.id(), 0u);

    // The contents of freshly allocated storage are undefined per spec (not guaranteed zero);
    // what the test pins down is that the old {1, 1} payload did not survive the resize, and
    // that the readback itself is a clean GL operation.
    std::array<SampleElement, 1> readBack{SampleElement{-1.f, -1.f}};
    glGetNamedBufferSubData(buffer.id(), 0, sizeof(readBack), readBack.data());
    expectNoGLError();

    EXPECT_FALSE(readBack[0].a == 1.f && readBack[0].b == 1.f)
        << "keepContents=false must not preserve the previous payload";
}

// ======== Binding ======== //

TEST(GLBuffer, BindBindingBindsToIndexedBindingPoint)
{
    GLBuffer<SampleElement> buffer{sizeof(SampleElement)};
    constexpr GLuint kBindingPoint = 3;

    buffer.bindBinding(GL_UNIFORM_BUFFER, kBindingPoint);

    GLint bound = 0;
    glGetIntegeri_v(GL_UNIFORM_BUFFER_BINDING, kBindingPoint, &bound);
    EXPECT_EQ(static_cast<GLuint>(bound), buffer.id());
    expectNoGLError();
}

TEST(GLBuffer, BindRangeBindsByteRangeToBindingPoint)
{
    GLBuffer<SampleElement> buffer{4 * sizeof(SampleElement)};
    constexpr GLuint kBindingPoint = 1;

    buffer.bindRange(GL_UNIFORM_BUFFER, kBindingPoint, 0, 2 * sizeof(SampleElement));

    GLint bound = 0;
    glGetIntegeri_v(GL_UNIFORM_BUFFER_BINDING, kBindingPoint, &bound);
    EXPECT_EQ(static_cast<GLuint>(bound), buffer.id());
    expectNoGLError();
}

TEST(GLBuffer, BindRangeAcceptsNonZeroOffset)
{
    // glBindBufferRange only accepts offsets that are multiples of the uniform-buffer offset
    // alignment (256 on most implementations), so build the range at a legal non-zero offset.
    GLint alignment = 0;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
    ASSERT_GT(alignment, 0);

    const auto offset = static_cast<std::size_t>(alignment);
    const std::size_t bytes = 2 * sizeof(SampleElement);
    GLBuffer<SampleElement> buffer{offset + bytes};
    constexpr GLuint kBindingPoint = 2;

    buffer.bindRange(GL_UNIFORM_BUFFER, kBindingPoint, offset, bytes);

    expectNoGLError();
}
