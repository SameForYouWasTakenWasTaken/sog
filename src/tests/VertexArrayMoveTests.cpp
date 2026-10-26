/**
 * @file VertexArrayMoveTests.cpp
 * @brief Documents gfx::VertexArray's move semantics, including the known quirk that a moved-from
 * VertexArray keeps its index bookkeeping (m_IndexCount is not reset by the move).
 *
 * A moved-from VertexArray can not be bound (its VAO handle was zeroed), but indexed() and
 * indexCount() still report the source's values. Tests pin this behavior down so an accidental
 * change (either direction) is caught.
 */

#include <TestSupport.hpp>

#include <array>
#include <span>
#include <type_traits>
#include <utility>

using sog::Index;
using sog::IndexBuffer;
using sog::VertexArray;
using sog::graphics_error;

namespace
{
/// Fills a VertexArray with a quad's index buffer so index bookkeeping is observable.
void attachQuadIndices(VertexArray& array)
{
    array.create();
    IndexBuffer indices;
    indices.set(std::span<const Index>{std::array<Index, 6>{0, 1, 2, 3, 4, 5}});
    array.setIndexBuffer(indices);
}
} // namespace

TEST(VertexArrayMove, MoveConstructorTransfersVao)
{
    VertexArray source;
    source.create();

    VertexArray target{std::move(source)};

    target.bind();
    EXPECT_TRUE(target.isBound());
    expectNoGLError();
}

TEST(VertexArrayMove, MovedFromVertexArrayCanNotBeBound)
{
    VertexArray source;
    source.create();

    VertexArray target{std::move(source)};

    EXPECT_THROW(source.bind(), graphics_error)
        << "the source must lose its VAO handle when moved";
    (void)target;
}

TEST(VertexArrayMove, MovedFromVertexArrayKeepsIndexBookkeeping)
{
    // Known quirk: the move transfers m_Handle but copies (rather than resets) m_IndexCount, so
    // the moved-from object still claims to be indexed.
    VertexArray source;
    attachQuadIndices(source);

    VertexArray target{std::move(source)};

    EXPECT_TRUE(target.indexed());
    EXPECT_EQ(target.indexCount(), 6u);

    EXPECT_TRUE(source.indexed()) << "moved-from VertexArray keeps its index count (documented quirk)";
    EXPECT_EQ(source.indexCount(), 6u) << "moved-from VertexArray keeps its index count";
}

TEST(VertexArrayMove, MoveAssignmentTransfersVaoAndBookkeeping)
{
    VertexArray source;
    attachQuadIndices(source);

    VertexArray target;
    target.create();
    target = std::move(source);

    target.bind();
    EXPECT_TRUE(target.isBound());
    EXPECT_EQ(target.indexCount(), 6u);

    EXPECT_THROW(source.bind(), graphics_error);
    expectNoGLError();
}

TEST(VertexArrayMove, SelfMoveAssignmentKeepsTheVertexArrayUsable)
{
    VertexArray array;
    attachQuadIndices(array);

    array = std::move(array);

    array.bind();
    EXPECT_TRUE(array.isBound());
    EXPECT_EQ(array.indexCount(), 6u);
    expectNoGLError();
}
