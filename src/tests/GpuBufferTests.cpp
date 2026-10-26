/**
 * @file GpuBufferTests.cpp
 * @brief Exercises gfx::GpuBuffer: the typed, growable buffer that layers element count/capacity
 * tracking over a backend::GLBuffer.
 *
 * Covers construction, set()/push() growth, update() in-place overwrite, clear(), reserve(),
 * the BindMode-constrained bind() overloads and id()/size()/capacity() accessors. A current GL
 * context is required (derived from GLTest).
 */

#include <TestSupport.hpp>

#include <array>
#include <cstddef>
#include <span>
#include <type_traits>
#include <vector>

#include <sog/gfx/OpenGL/Objects/Buffers/GpuBuffer.hpp>

using sog::gfx::BindingMode;
using sog::gfx::GpuBuffer;

namespace
{
/// Element type for the test buffers; trivially copyable, like real payloads.
struct Light
{
    float intensity = 0.f;
    float radius = 0.f;
};

/// A Regular-mode buffer bound to the array-buffer target (the common vertex-buffer shape).
using ArrayGpuBuffer = GpuBuffer<Light, GL_ARRAY_BUFFER, BindingMode::Regular>;

/// An Indexed-mode buffer bound to the uniform-buffer target (binds to a binding point).
using IndexedUniformBuffer = GpuBuffer<Light, GL_UNIFORM_BUFFER, BindingMode::Indexed>;

/// Reads back the whole buffer's elements from the GPU for content assertions.
std::vector<Light> readBack(const GpuBuffer<Light, GL_ARRAY_BUFFER, BindingMode::Regular>& buffer)
{
    std::vector<Light> out(buffer.size());
    if (!out.empty())
        glGetNamedBufferSubData(buffer.id(), 0, out.size() * sizeof(Light), out.data());
    return out;
}
} // namespace

// ======== Type properties ======== //

TEST(GpuBuffer, IsMoveOnly)
{
    static_assert(!std::is_copy_constructible_v<ArrayGpuBuffer>,
                  "GpuBuffer must not be copy constructible");
    static_assert(!std::is_copy_assignable_v<ArrayGpuBuffer>,
                  "GpuBuffer must not be copy assignable");
    static_assert(std::is_nothrow_move_constructible_v<ArrayGpuBuffer>,
                  "GpuBuffer must be nothrow move constructible");
    static_assert(std::is_nothrow_move_assignable_v<ArrayGpuBuffer>,
                  "GpuBuffer must be nothrow move assignable");
    SUCCEED();
}

// ======== Construction ======== //

TEST(GpuBuffer, DefaultConstructionYieldsEmptyBuffer)
{
    const ArrayGpuBuffer buffer;

    EXPECT_EQ(buffer.size(), 0u);
    EXPECT_EQ(buffer.capacity(), 0u);
    EXPECT_EQ(buffer.id(), 0u) << "an empty buffer should not allocate a GL object yet";
}

TEST(GpuBuffer, ConstructingWithCapacityReservesStorage)
{
    const ArrayGpuBuffer buffer{8};

    EXPECT_EQ(buffer.size(), 0u);
    EXPECT_EQ(buffer.capacity(), 8u);
    EXPECT_NE(buffer.id(), 0u);
}

// ======== set() ======== //

TEST(GpuBuffer, SetUploadsContentsAndGrows)
{
    const std::array<Light, 3> payload{Light{1.f, 10.f}, Light{2.f, 20.f}, Light{3.f, 30.f}};
    ArrayGpuBuffer buffer;
    buffer.set(std::span<const Light>{payload});

    EXPECT_EQ(buffer.size(), 3u);
    EXPECT_GE(buffer.capacity(), 3u);

    const auto back = readBack(buffer);
    ASSERT_EQ(back.size(), 3u);
    EXPECT_FLOAT_EQ(back[0].intensity, 1.f);
    EXPECT_FLOAT_EQ(back[1].radius, 20.f);
    EXPECT_FLOAT_EQ(back[2].intensity, 3.f);
}

TEST(GpuBuffer, SetWithSmallerRangeShrinksSizeButKeepsCapacity)
{
    const std::array<Light, 8> big{Light{1.f, 1.f}};
    ArrayGpuBuffer buffer;
    buffer.set(std::span<const Light>{big});
    const size_t capacityBefore = buffer.capacity();

    const std::array<Light, 2> small{Light{9.f, 9.f}, Light{8.f, 8.f}};
    buffer.set(std::span<const Light>{small});

    EXPECT_EQ(buffer.size(), 2u);
    EXPECT_EQ(buffer.capacity(), capacityBefore) << "capacity must not shrink on set()";
}

TEST(GpuBuffer, SetWithEmptyRangeYieldsZeroSize)
{
    ArrayGpuBuffer buffer{4};
    buffer.set(std::span<const Light>{});

    EXPECT_EQ(buffer.size(), 0u);
}

// ======== push() ======== //

TEST(GpuBuffer, PushAppendsAndGrowsFromZeroCapacity)
{
    ArrayGpuBuffer buffer;
    buffer.push(Light{1.f, 1.f});
    buffer.push(Light{2.f, 2.f});

    EXPECT_EQ(buffer.size(), 2u);
    EXPECT_GE(buffer.capacity(), 2u);

    const auto back = readBack(buffer);
    ASSERT_EQ(back.size(), 2u);
    EXPECT_FLOAT_EQ(back[0].intensity, 1.f);
    EXPECT_FLOAT_EQ(back[1].intensity, 2.f);
}

TEST(GpuBuffer, PushGrowsGeometricallyWhenFull)
{
    ArrayGpuBuffer buffer{2};
    buffer.push(Light{1.f, 0.f});
    buffer.push(Light{2.f, 0.f});
    const size_t capacityAfterTwo = buffer.capacity();

    buffer.push(Light{3.f, 0.f});

    EXPECT_EQ(buffer.size(), 3u);
    EXPECT_GT(buffer.capacity(), capacityAfterTwo) << "capacity must grow once full";
}

TEST(GpuBuffer, PushPreservesExistingContentsAcrossGrowth)
{
    ArrayGpuBuffer buffer;
    for (int i = 0; i < 17; ++i)
        buffer.push(Light{static_cast<float>(i), 0.f});

    EXPECT_EQ(buffer.size(), 17u);

    const auto back = readBack(buffer);
    ASSERT_EQ(back.size(), 17u);
    for (int i = 0; i < 17; ++i)
        EXPECT_FLOAT_EQ(back[static_cast<std::size_t>(i)].intensity, static_cast<float>(i));
}

// ======== update() ======== //

TEST(GpuBuffer, UpdateOverwritesSubRangeInPlace)
{
    const std::array<Light, 3> payload{Light{1.f, 0.f}, Light{2.f, 0.f}, Light{3.f, 0.f}};
    ArrayGpuBuffer buffer;
    buffer.set(std::span<const Light>{payload});

    const std::array<Light, 1> patched{Light{99.f, 42.f}};
    buffer.update(1, std::span<const Light>{patched});

    const auto back = readBack(buffer);
    ASSERT_EQ(back.size(), 3u);
    EXPECT_FLOAT_EQ(back[0].intensity, 1.f);
    EXPECT_FLOAT_EQ(back[1].intensity, 99.f);
    EXPECT_FLOAT_EQ(back[1].radius, 42.f);
    EXPECT_FLOAT_EQ(back[2].intensity, 3.f);
}

TEST(GpuBuffer, UpdateDoesNotChangeSizeOrCapacity)
{
    const std::array<Light, 4> payload{Light{1.f, 1.f}};
    ArrayGpuBuffer buffer;
    buffer.set(std::span<const Light>{payload});
    const size_t sizeBefore = buffer.size();
    const size_t capacityBefore = buffer.capacity();

    const std::array<Light, 2> patched{Light{5.f, 5.f}, Light{6.f, 6.f}};
    buffer.update(0, std::span<const Light>{patched});

    EXPECT_EQ(buffer.size(), sizeBefore);
    EXPECT_EQ(buffer.capacity(), capacityBefore);
}

// ======== clear() and reserve() ======== //

TEST(GpuBuffer, ClearResetsSizeButKeepsCapacity)
{
    const std::array<Light, 4> payload{Light{1.f, 1.f}};
    ArrayGpuBuffer buffer;
    buffer.set(std::span<const Light>{payload});
    const size_t capacityBefore = buffer.capacity();

    buffer.clear();

    EXPECT_EQ(buffer.size(), 0u);
    EXPECT_EQ(buffer.capacity(), capacityBefore);
}

TEST(GpuBuffer, ReserveGrowsCapacityWithoutTouchingSize)
{
    ArrayGpuBuffer buffer;
    buffer.reserve(32);

    EXPECT_EQ(buffer.size(), 0u);
    EXPECT_EQ(buffer.capacity(), 32u);
    EXPECT_NE(buffer.id(), 0u);
}

TEST(GpuBuffer, ReserveSmallerThanCurrentCapacityIsANoOp)
{
    ArrayGpuBuffer buffer{16};
    buffer.reserve(4);

    EXPECT_EQ(buffer.capacity(), 16u);
}

// ======== bind() overloads ======== //

TEST(GpuBuffer, RegularBufferBindsToTarget)
{
    ArrayGpuBuffer buffer{4};
    buffer.bind();

    GLint bound = 0;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &bound);
    EXPECT_EQ(static_cast<GLuint>(bound), buffer.id());
    expectNoGLError();
}

TEST(GpuBuffer, IndexedBufferBindsToBindingPoint)
{
    IndexedUniformBuffer buffer{4};
    constexpr GLuint kBindingPoint = 5;

    buffer.bind(kBindingPoint);

    GLint bound = 0;
    glGetIntegeri_v(GL_UNIFORM_BUFFER_BINDING, kBindingPoint, &bound);
    EXPECT_EQ(static_cast<GLuint>(bound), buffer.id());
    expectNoGLError();
}

/// Probes whether a buffer type exposes the no-argument (fixed-target) bind() overload.
template <typename Buffer>
concept BindsWithNoArgument = requires(const Buffer& buffer) { buffer.bind(); };

/// Probes whether a buffer type exposes the bind(bindingPoint) overload for indexed targets.
template <typename Buffer>
concept BindsWithBindingPoint = requires(const Buffer& buffer) { buffer.bind(0u); };

TEST(GpuBuffer, BindOverloadIsSelectedByBindingMode)
{
    // Regular-mode buffers expose only bind(); Indexed-mode buffers expose only bind(binding).
    // The probes are concepts (dependent requirements), so an unmet requirement evaluates to
    // false rather than failing to compile the way a non-dependent requires-expression would.
    static_assert(BindsWithNoArgument<ArrayGpuBuffer>);
    static_assert(!BindsWithBindingPoint<ArrayGpuBuffer>);
    static_assert(BindsWithBindingPoint<IndexedUniformBuffer>);
    static_assert(!BindsWithNoArgument<IndexedUniformBuffer>);
    SUCCEED();
}
