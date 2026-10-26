/**
 * @file ClearBufferTests.cpp
 * @brief Exercises gfx::ClearBuffer: the GL buffer-bit flags and their bitwise-or combinator.
 */

#include <TestSupport.hpp>

#include <type_traits>

using sog::ClearBuffer;
using sog::to_underlying;

TEST(ClearBuffer, UnderlyingValuesMatchGLBits)
{
    EXPECT_EQ(to_underlying(ClearBuffer::Color), static_cast<unsigned int>(GL_COLOR_BUFFER_BIT));
    EXPECT_EQ(to_underlying(ClearBuffer::Depth), static_cast<unsigned int>(GL_DEPTH_BUFFER_BIT));
    EXPECT_EQ(to_underlying(ClearBuffer::Stencil),
              static_cast<unsigned int>(GL_STENCIL_BUFFER_BIT));
}

TEST(ClearBuffer, OrCombinesColorAndDepth)
{
    const auto combined = ClearBuffer::Color | ClearBuffer::Depth;

    EXPECT_EQ(to_underlying(combined),
              static_cast<unsigned int>(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

TEST(ClearBuffer, OrCombinesAllThree)
{
    const auto combined = ClearBuffer::Color | ClearBuffer::Depth | ClearBuffer::Stencil;

    EXPECT_EQ(to_underlying(combined),
              static_cast<unsigned int>(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                                        GL_STENCIL_BUFFER_BIT));
}

TEST(ClearBuffer, OrIsCommutative)
{
    EXPECT_EQ(to_underlying(ClearBuffer::Color | ClearBuffer::Depth),
              to_underlying(ClearBuffer::Depth | ClearBuffer::Color));
}

TEST(ClearBuffer, OrWithItselfIsIdempotent)
{
    const auto combined = ClearBuffer::Depth | ClearBuffer::Depth;

    EXPECT_EQ(to_underlying(combined), to_underlying(ClearBuffer::Depth));
}

TEST(ClearBuffer, OrResultRemainsAClearBuffer)
{
    // The combinator must return the enum type, not a raw integer, so it stays type-safe at the
    // call site (e.g. window.beginFrame(ClearBuffer::Color | ClearBuffer::Depth)).
    static_assert(std::is_same_v<decltype(ClearBuffer::Color | ClearBuffer::Depth), ClearBuffer>,
                  "operator| must return ClearBuffer, not an integer");
    SUCCEED();
}
