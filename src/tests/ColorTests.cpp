/**
 * @file ColorTests.cpp
 * @brief Exercises gfx::Color: the 0-255 RGBA value type, its normalization for GL, and the
 * 8-bit conversion helpers.
 */

#include <TestSupport.hpp>

using sog::Color;

namespace
{
/// Tolerance for float comparisons against 1/255-derived values.
constexpr float TOLERANCE = 1e-6f;
} // namespace

TEST(Color, DefaultsToOpaqueBlack)
{
    const Color color{};

    EXPECT_FLOAT_EQ(color.r, 0.f);
    EXPECT_FLOAT_EQ(color.g, 0.f);
    EXPECT_FLOAT_EQ(color.b, 0.f);
    EXPECT_FLOAT_EQ(color.a, Color::MAX_VALUE) << "alpha defaults to opaque, not transparent";
}

TEST(Color, RgbOnlyLiteralStaysOpaque)
{
    // Color{255, 0, 0} must be fully visible red — omitting alpha must not make it transparent.
    const Color color{255.f, 0.f, 0.f};

    EXPECT_FLOAT_EQ(color.r, 255.f);
    EXPECT_FLOAT_EQ(color.a, 255.f);
}

TEST(Color, AggregateInitializationPreservesChannels)
{
    const Color color{12.5f, 64.25f, 255.f, 127.5f};

    EXPECT_FLOAT_EQ(color.r, 12.5f);
    EXPECT_FLOAT_EQ(color.g, 64.25f);
    EXPECT_FLOAT_EQ(color.b, 255.f);
    EXPECT_FLOAT_EQ(color.a, 127.5f);
}

TEST(Color, FromRGBARetainsTheEightBitScale)
{
    // fromRGBA() stores the 8-bit values verbatim on Color's native 0-255 scale.
    const Color color = Color::fromRGBA(255, 128, 0, 255);

    EXPECT_NEAR(color.r, 255.f, TOLERANCE);
    EXPECT_NEAR(color.g, 128.f, TOLERANCE);
    EXPECT_NEAR(color.b, 0.f, TOLERANCE);
    EXPECT_NEAR(color.a, 255.f, TOLERANCE);
}

TEST(Color, RoundTripsThroughToRGBA)
{
    const auto bytes = Color::fromRGBA(200, 100, 50, 255).toRGBA();

    EXPECT_EQ(bytes[0], 200);
    EXPECT_EQ(bytes[1], 100);
    EXPECT_EQ(bytes[2], 50);
    EXPECT_EQ(bytes[3], 255);
}

TEST(Color, NormalizedScalesChannelsIntoTheGlRange)
{
    // Internals hand colors to GL through normalized(): 0-255 in, [0, 1] out.
    const sog::math::Vec4 n = Color{255.f, 128.f, 0.f, 255.f}.normalized();

    EXPECT_NEAR(n.x, 1.f, TOLERANCE);
    EXPECT_NEAR(n.y, 128.f / 255.f, TOLERANCE);
    EXPECT_NEAR(n.z, 0.f, TOLERANCE);
    EXPECT_NEAR(n.w, 1.f, TOLERANCE);
}

TEST(Color, ExplicitVec4ConversionMatchesNormalized)
{
    const Color color{255.f, 0.f, 127.5f, 255.f};
    const sog::math::Vec4 vec = static_cast<sog::math::Vec4>(color);

    EXPECT_NEAR(vec.x, 1.f, TOLERANCE);
    EXPECT_NEAR(vec.y, 0.f, TOLERANCE);
    EXPECT_NEAR(vec.z, 0.5f, TOLERANCE);
    EXPECT_NEAR(vec.w, 1.f, TOLERANCE);
}

TEST(Color, ClampsOutOfRangeChannelsWhenConvertingToBytes)
{
    const Color clampedHigh{300.f, 255.f, -10.f, 0.f};
    const auto high = clampedHigh.toRGBA();

    EXPECT_EQ(high[0], 255);
    EXPECT_EQ(high[1], 255);
    EXPECT_EQ(high[2], 0);
    EXPECT_EQ(high[3], 0);

    const Color clampedLow{-5.f, -0.001f, 0.f, 260.f};
    const auto low = clampedLow.toRGBA();

    EXPECT_EQ(low[0], 0);
    EXPECT_EQ(low[1], 0);
    EXPECT_EQ(low[2], 0);
    EXPECT_EQ(low[3], 255);
}

TEST(Color, ToRgbaRoundsToTheNearestByte)
{
    // Locals first: a Color{...} brace list inside EXPECT_EQ would split the macro's arguments.
    const auto belowHalf = Color{127.4f, 0.f, 0.f, 255.f}.toRGBA();
    const auto halfWay = Color{127.5f, 0.f, 0.f, 255.f}.toRGBA();
    const auto aboveHalf = Color{128.6f, 0.f, 0.f, 255.f}.toRGBA();

    EXPECT_EQ(belowHalf[0], 127);
    EXPECT_EQ(halfWay[0], 128);
    EXPECT_EQ(aboveHalf[0], 129);
}

TEST(Color, MaxValueIsTwentyFiveFive)
{
    EXPECT_FLOAT_EQ(Color::MAX_VALUE, 255.f);
}
