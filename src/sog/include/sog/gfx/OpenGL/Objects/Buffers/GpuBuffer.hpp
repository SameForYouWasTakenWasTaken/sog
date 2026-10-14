/**
 * @file GpuBuffer.hpp
 * @brief Defines GpuBuffer<T, Target, BindMode>, a growable typed buffer that keeps a count/capacity view over a backend::GLBuffer.
 */

#pragma once
#include <cstdint>
#include <span>
#include <type_traits>

#include "GLBuffer.hpp"

namespace sog::gfx
{

/**
 * @brief How a GpuBuffer is bound to the pipeline.
 */
enum class BindingMode
{
    Regular, /**< Bound with the classic bind-to-target model (glBindBuffer); uses bind(). */
    Indexed  /**< Bound to an indexed binding point (glBindBufferBase); uses bind(binding). */
};

/**
 * @brief Typed, automatically growing GPU buffer with element-level accessors.
 *
 * Wraps backend::GLBuffer (byte storage with immutable GL 4.5 allocation) and
 * adds an element count and capacity. set() replaces the whole contents and grows
 * the buffer as needed; push() appends, growing geometrically; update() overwrites
 * an existing sub-range in place. Capacity is measured in elements, storage in bytes.
 *
 * The bind() overload available depends on BindMode (a C++20 requires clause
 * selects one): Regular buffers bind to a target, Indexed buffers bind to a
 * binding point. Element type T must be trivially copyable (enforced by
 * static_assert) because it is uploaded to the GPU as raw bytes.
 *
 * @tparam T Element type stored in the buffer (e.g. Vertex, or a uniform struct).
 * @tparam Target GL target to bind to (GL_ARRAY_BUFFER, GL_UNIFORM_BUFFER, ...).
 * @tparam BindMode BindingMode::Regular or BindingMode::Indexed.
 *
 * @attention set() with a smaller range shrinks the logical size but not the
 * GPU allocation; use reserve() only to grow, and note clear() resets the element
 * count without touching stored data.
 *
 * @see sog::VertexBuffer, sog::IndexBuffer, sog::UniformBuffer (Buffers.hpp)
 */
template <typename T, GLenum Target, BindingMode BindMode> class GpuBuffer
{
    static_assert(std::is_trivially_copyable_v<T>, "GPU data must be plain bytes");

  public:
    /**
     * @brief Constructs a buffer, optionally reserving initial capacity.
     * @param capacity Initial capacity in elements; 0 creates an empty buffer.
     */
    explicit GpuBuffer(size_t capacity = 0) { reserve(capacity); }

    /**
     * @brief Replaces the whole contents with the given elements, growing if needed.
     * @param data Elements to upload; becomes the new logical contents.
     *
     * Growth discards old contents (keep=false) since everything is overwritten anyway.
     */
    // Replace the whole contents; grows automatically
    void set(std::span<const T> data)
    {
        if (data.size() > capacity_)
            grow(data.size(), /*keep=*/false);
        buf_.write(0, data.data(), data.size_bytes());
        count_ = data.size();
    }

    /**
     * @brief Overwrites an existing sub-range in place (e.g. one dirty light).
     * @param first Index of the first element to overwrite.
     * @param data New values; must fit within the current element count.
     */
    // Overwrite a sub-range (e.g. one dirty light)
    void update(size_t first, std::span<const T> data)
    {
        assert(first + data.size() <= count_);
        buf_.write(first * sizeof(T), data.data(), data.size_bytes());
    }

    /**
     * @brief Appends one element, growing capacity geometrically when full.
     * @param v Element to append.
     *
     * On overflow the buffer grows to max(16, capacity * 2), keeping existing contents.
     */
    void push(const T& v)
    {
        if (count_ == capacity_)
            grow(std::max<size_t>(16, capacity_ * 2), /*keep=*/true);
        buf_.write(count_ * sizeof(T), &v, sizeof(T));
        ++count_;
    }

    /**
     * @brief Ensures capacity for at least n elements, without touching contents.
     * @param n Minimum capacity in elements.
     */
    void reserve(size_t n)
    {
        if (n > capacity_)
            grow(n, true);
    }

    /// Resets the logical element count to 0; storage and contents are kept.
    void clear() { count_ = 0; }

    /**
     * @brief Binds the whole buffer to its indexed binding point (Indexed mode only).
     * @param binding Index of the binding point (e.g. a uniform block index).
     */
    void bind(GLuint binding) const
        requires(BindMode == BindingMode::Indexed)
    {
        buf_.bindBinding(Target, binding);
    }

    /**
     * @brief Binds the buffer to its GL target (Regular mode only).
     */
    void bind() const
        requires(BindMode == BindingMode::Regular)
    {
        buf_.bindBuffer();
    }

    /**
     * @brief Returns the number of live elements.
     * @return Element count (set via set()/push(), reset by clear()).
     */
    [[nodiscard]] size_t size() const { return count_; }

    /**
     * @brief Returns the allocated capacity in elements.
     * @return Number of elements that fit before the next growth.
     */
    [[nodiscard]] size_t capacity() const { return capacity_; }

    /**
     * @brief Returns the raw GL buffer name for advanced interop.
     * @return GLuint handle of the underlying GLBuffer.
     */
    [[nodiscard]] GLuint id() const { return buf_.id(); }

  private:
    /**
     * @brief Grows (or creates) the underlying byte buffer to newCap elements.
     * @param newCap New capacity in elements.
     * @param keep Whether to preserve existing contents across the resize.
     */
    void grow(size_t newCap, bool keep)
    {
        buf_.resize(newCap * sizeof(T), keep);
        capacity_ = newCap;
    }

    backend::GLBuffer<T> buf_; /**< Byte-level GL storage backing this buffer. */
    size_t count_ = 0, capacity_ = 0; /**< Live element count and allocated element capacity. */
};
} // namespace sog::gfx
