/**
 * @file VertexArrayTests.cpp
 * @brief Exercises gfx::VertexArray: VAO lifecycle, DSA buffer/attribute setup, index bookkeeping
 * and bind-state queries.
 *
 * Requires a current GL context (derives from GLTest). The pixel-level proof that the configured
 * layout actually feeds a draw call lives in CommonDrawTests.cpp; this file covers the object's
 * own behavior.
 */

#include <TestSupport.hpp>

#include <array>
#include <span>
#include <type_traits>

using sog::Index;
using sog::IndexBuffer;
using sog::VertexArray;
using sog::VertexBuffer;
using sog::graphics_error;
using sog::graphics_error_code;

namespace
{
/// Four indices, forming two triangles out of a quad.
constexpr std::array<Index, 6> QUAD_INDICES{0, 1, 2, 3, 4, 5};
} // namespace

// ======== Type properties ======== //

TEST(VertexArray, IsMoveOnly)
{
    static_assert(!std::is_copy_constructible_v<VertexArray>,
                  "VertexArray must not be copy constructible");
    static_assert(!std::is_copy_assignable_v<VertexArray>,
                  "VertexArray must not be copy assignable");
    static_assert(std::is_nothrow_move_constructible_v<VertexArray>,
                  "VertexArray must be nothrow move constructible");
    static_assert(std::is_nothrow_move_assignable_v<VertexArray>,
                  "VertexArray must be nothrow move assignable");
    SUCCEED();
}

// ======== Construction / destruction ======== //

TEST(VertexArray, DefaultConstructedHasNoGLObject)
{
    const VertexArray array;

    EXPECT_FALSE(array.indexed());
    EXPECT_EQ(array.indexCount(), 0u);
    EXPECT_FALSE(array.isBound());
}

TEST(VertexArray, BindOnDefaultConstructedVertexArrayThrows)
{
    const VertexArray array;

    try
    {
        array.bind();
        FAIL() << "binding a VAO that was never created must throw";
    }
    catch (const graphics_error& error)
    {
        EXPECT_EQ(error.code(), graphics_error_code::buffer_creation_failed);
    }
}

TEST(VertexArray, CreateMakesTheVertexArrayBindable)
{
    VertexArray array;
    array.create();
    array.bind();

    EXPECT_TRUE(array.isBound());
    expectNoGLError();
}

TEST(VertexArray, CreateTwiceIsANoOp)
{
    VertexArray array;
    array.create();
    array.create();
    array.bind();

    EXPECT_TRUE(array.isBound());
    expectNoGLError();
}

TEST(VertexArray, UnbindClearsTheBoundState)
{
    VertexArray array;
    array.create();
    array.bind();

    VertexArray::unbind();

    EXPECT_FALSE(array.isBound());
    expectNoGLError();
}

TEST(VertexArray, ReleaseThenBindThrows)
{
    VertexArray array;
    array.create();
    array.release();

    EXPECT_THROW(array.bind(), graphics_error);
}

// ======== Buffer wiring (direct state access) ======== //

TEST(VertexArray, SetVertexBufferWorksWhileUnbound)
{
    // All set* methods are DSA: they must succeed without the VAO being bound first.
    VertexArray array;
    array.create();
    const auto vertices = sogtest::fullScreenQuad({1.f, 0.f, 0.f, 1.f});
    VertexBuffer buffer;
    buffer.set(std::span<const sog::Vertex>{vertices});

    array.setVertexBuffer(buffer, 0, sog::Vertex::stride());

    EXPECT_FALSE(array.isBound());
    expectNoGLError();
}

TEST(VertexArray, SetAttributesWorksWhileUnbound)
{
    VertexArray array;
    array.create();
    const auto vertices = sogtest::fullScreenQuad({1.f, 0.f, 0.f, 1.f});
    VertexBuffer buffer;
    buffer.set(std::span<const sog::Vertex>{vertices});
    array.setVertexBuffer(buffer, 0, sog::Vertex::stride());

    array.setAttributes(sog::Vertex::attributes());

    expectNoGLError();
}

// ======== Index bookkeeping ======== //

TEST(VertexArray, IsNotIndexedByDefault)
{
    VertexArray array;
    array.create();

    EXPECT_FALSE(array.indexed());
    EXPECT_EQ(array.indexCount(), 0u);
}

TEST(VertexArray, SetIndexBufferRecordsElementCount)
{
    VertexArray array;
    array.create();

    IndexBuffer indices;
    indices.set(std::span<const Index>{QUAD_INDICES});
    array.setIndexBuffer(indices);

    EXPECT_TRUE(array.indexed());
    EXPECT_EQ(array.indexCount(), QUAD_INDICES.size());
    expectNoGLError();
}

TEST(VertexArray, SetIndexBufferWithEmptyBufferStaysNonIndexed)
{
    VertexArray array;
    array.create();

    IndexBuffer emptyIndices;
    array.setIndexBuffer(emptyIndices);

    EXPECT_EQ(array.indexCount(), 0u);
    EXPECT_FALSE(array.indexed());
}
