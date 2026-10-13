#pragma once
#include <cstdint>
#include <span>
#include <type_traits>

#include "gfx/backend/GLBuffer.hpp"

namespace sog::gfx
{

enum class BindingMode
{
    Regular,
    Indexed
};
template <typename T, GLenum Target, BindingMode BindMode> class GpuBuffer
{
    static_assert(std::is_trivially_copyable_v<T>, "GPU data must be plain bytes");

  public:
    explicit GpuBuffer(size_t capacity = 0) { reserve(capacity); }

    // Replace the whole contents; grows automatically
    void set(std::span<const T> data)
    {
        if (data.size() > capacity_)
            grow(data.size(), /*keep=*/false);
        buf_.write(0, data.data(), data.size_bytes());
        count_ = data.size();
    }

    // Overwrite a sub-range (e.g. one dirty light)
    void update(size_t first, std::span<const T> data)
    {
        assert(first + data.size() <= count_);
        buf_.write(first * sizeof(T), data.data(), data.size_bytes());
    }

    void push(const T& v)
    {
        if (count_ == capacity_)
            grow(std::max<size_t>(16, capacity_ * 2), /*keep=*/true);
        buf_.write(count_ * sizeof(T), &v, sizeof(T));
        ++count_;
    }

    void reserve(size_t n)
    {
        if (n > capacity_)
            grow(n, true);
    }
    void clear() { count_ = 0; }

    void bind(GLuint binding) const
        requires(BindMode == BindingMode::Indexed)
    {
        buf_.bindBinding(Target, binding);
    }
    void bind() const
        requires(BindMode == BindingMode::Regular)
    {
        buf_.bindBuffer();
    }

    [[nodiscard]] size_t size() const { return count_; }
    [[nodiscard]] size_t capacity() const { return capacity_; }
    [[nodiscard]] GLuint id() const { return buf_.id(); }

  private:
    void grow(size_t newCap, bool keep)
    {
        buf_.resize(newCap * sizeof(T), keep);
        capacity_ = newCap;
    }

    backend::GLBuffer buf_;
    size_t count_ = 0, capacity_ = 0;
};
} // namespace sog::gfx