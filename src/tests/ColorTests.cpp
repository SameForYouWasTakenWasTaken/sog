/**
 * @file ColorTests.cpp
 * @brief Exercises gfx::Color: the float RGBA value type and its 8-bit conversion helpers.
 */

#include <TestSupport.hpp>

using sog::Color;

namespace
{
/// Tolerance for float comparisons against 1/255-derived values.
constexpr float TOLERANCE = 1e-6f;
} // namespace

TEST(Color, DefaultsToTransparentBlack)
{
    const Color color{};

    EXPECT_FLOAT_EQ(color.r, 0.f);
    EXPECT_FLOAT_EQ(color.g, 0.f);
    EXPECT_FLOAT_EQ(color.b, 0.f);
    EXPECT_FLOAT_EQ(color.a, 0.f);
}

TEST(Color, AggregateInitializationPreservesChannels)
{
    const Color color{0.25f, 0.5f, 0.75f, 1.f};

    EXPECT_FLOAT_EQ(color.r, 0.25f);
    EXPECT_FLOAT_EQ(color.g, 0.5f);
    EXPECT_FLOAT_EQ(color.b, 0.75f);
    EXPECT_FLOAT_EQ(color.a, 1.f);
}

TEST(Color, FromRGBAScalesToNormalizedRange)
{
    const Color color = Color::fromRGBA(255, 128, 0, 255);

    EXPECT_NEAR(color.r, 1.f, TOLERANCE);
    EXPECT_NEAR(color.g, 128.f / 255.f, TOLERANCE);
    EXPECT_NEAR(color.b, 0.f, TOLERANCE);
    EXPECT_NEAR(color.a, 1.f, TOLERANCE);
}

TEST(Color, FromRGBABlackAndWhite)
{
    const Color black = Color::fromRGBA(0, 0, 0, 0);
    const Color white = Color::fromRGBA(255, 255, 255, 255);

    EXPECT_FLOAT_EQ(black.r, 0.f);
    EXPECT_FLOAT_EQ(black.a, 0.f);
    EXPECT_NEAR(white.r, 1.f, TOLERANCE);
    EXPECT_NEAR(white.a, 1.f, TOLERANCE);
}

TEST(Color, ToRGBARoundTripsEightBitValues)
{
    const auto bytes = Color::fromRGBA(200, 100, 50, 255).toRGBA();

    EXPECT_EQ(bytes[0], 200);
    EXPECT_EQ(bytes[1], 100);
    EXPECT_EQ(bytes[2], 50);
    EXPECT_EQ(bytes[3], 255);
}

TEST(Color, ToRGBAClampsOutOfRangeChannels)
{
    const Color clampedHigh{2.f, 3.f, 1.5f, 1.f};
    const Color clampedLow{-1.f, -0.5f, 0.f, 0.f};

    const auto high = clampedHigh.toRGBA();
    EXPECT_EQ(high[0], 255);
    EXPECT_EQ(high[1], 255);
    EXPECT_EQ(high[2], 255);

    const auto low = clampedLow.toRGBA();
    EXPECT_EQ(low[0], 0);
    EXPECT_EQ(low[1], 0);
    EXPECT_EQ(low[2], 0);
}

TEST(Color, ToRGBAConvertsNormalizedChannelsBackToBytes)
{
    const auto bytes = Color{1.f, 0.f, 0.f, 1.f}.toRGBA();

    EXPECT_EQ(bytes[0], 255);
    EXPECT_EQ(bytes[1], 0);
    EXPECT_EQ(bytes[2], 0);
    EXPECT_EQ(bytes[3], 255);
}

TEST(Color, ExplicitVec4ConversionPreservesChannels)
{
    const Color color{0.1f, 0.2f, 0.3f, 0.4f};
    const sog::math::Vec4 vec = static_cast<sog::math::Vec4>(color);

    EXPECT_FLOAT_EQ(vec.x, 0.1f);
    EXPECT_FLOAT_EQ(vec.y, 0.2f);
    EXPECT_FLOAT_EQ(vec.z, 0.3f);
    EXPECT_FLOAT_EQ(vec.w, 0.4f);
}

TEST(Color, MaxValueIsTwentyFiveFive)
{
    EXPECT_FLOAT_EQ(Color::MAX_VALUE, 255.f);
}
