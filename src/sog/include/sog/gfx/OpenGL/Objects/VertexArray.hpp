/**
 * @file VertexArray.hpp
 * @brief Defines VertexArray, a RAII wrapper around a GL vertex array object that binds
 * vertex/index buffers and configures attribute formats.
 */

#pragma once
#include <sog/gfx/OpenGL/Objects/Buffers/Buffers.hpp>
#include <sog/gfx/OpenGL/Objects/VertexArrayAttributes.hpp>
#include <sog/gfx/Types/Handles.hpp>
#include <span>
#include <vector>

#include "glad/gl.h"
#include "sog/gfx/Types/Enums.hpp"

namespace sog::gfx
{

/**
 * @brief Owns a GL vertex array object (VAO) and configures how vertex data is read.
 *
 * create() allocates the VAO (no-op if one already exists). The set* methods use
 * GL's direct-state-access entry points to attach a vertex buffer, attach an index
 * buffer and apply a set of attribute formats without requiring the VAO to be
 * bound first. bind() makes the VAO current; the constructor/destructor manage the
 * GL object's lifetime.
 *
 * Attaching an index buffer via setIndexBuffer() also records the element count,
 * which indexed() and indexCount() expose; Window::draw() uses this to choose
 * glDrawElements over glDrawArrays.
 *
 * The class is move-only; the GL object is deleted on destruction or release().
 *
 * @see sog::VertexArrayAttributes, sog::VertexBuffer, sog::IndexBuffer
 */
class VertexArray
{
  public:
    /// Constructs an empty VertexArray with no GL object allocated.
    VertexArray();

    /// Destroys the owned VAO (if any).
    ~VertexArray();

    /**
     * @brief Move-constructs from another VertexArray, taking over its VAO.
     * @param o VertexArray to move from; left without a VAO.
     */
    VertexArray(VertexArray&& o) noexcept
        : m_Handle(std::move(o.m_Handle)), m_IndexCount(o.m_IndexCount), m_IndexType(o.m_IndexType)
    {
    }

    /**
     * @brief Move-assigns, releasing the current VAO and taking the source's.
     * @param o VertexArray to move from.
     * @return Reference to this VertexArray.
     */
    auto& operator=(VertexArray&& o) noexcept
    {
        if (this != &o)
        {
            release();
            m_Handle = std::move(o.m_Handle);
            m_IndexCount = o.m_IndexCount;
            m_IndexType = o.m_IndexType;
        }
        return *this;
    }

    /**
     * @brief Creates the GL vertex array object (no-op if already created).
     */
    void create();

    /**
     * @brief Binds this VAO as the current one (glBindVertexArray).
     * @throws sog::graphics_error If the underlying handle is invalid.
     */
    void bind() const;

    /**
     * @brief Attaches a buffer to a binding point of this VAO.
     * @param buffer Buffer supplying the data.
     * @param binding Binding point index to attach to.
     * @param stride Byte distance between consecutive vertices.
     */
    template <typename Tag>
    void setVertexBuffer(const GpuBuffer<Tag, GL_ARRAY_BUFFER, BindingMode::Regular>& buffer,
                         GLuint binding, GLsizei stride)
    {
        glVertexArrayVertexBuffer(m_Handle.get(), binding, buffer.id(), 0, stride);
    }

    /**
     * @brief Attaches an index buffer to this VAO and records its element count. Won't attach if
     * index buffer contains no indices
     * @tparam Tag The tag for the index buffer (i.e what data it holds)
     * @param indexBuffer Index buffer holding the element indices (must not be empty).
     * @param indexType The index type.
     * @throws graphics_error If GL handle of buffer or this VAO is invalid
     */
    template <typename Tag>
    void
    setIndexBuffer(const GpuBuffer<Tag, GL_ELEMENT_ARRAY_BUFFER, BindingMode::Regular>& indexBuffer,
                   index_type indexType = index_type::UINT);

    /**
     * @brief Updates index count from buffer.
     * @tparam Tag The tag for the index buffer (i.e what data it holds)
     * @param indexBuffer Index buffer holding the element indices (must not be empty).
     */
    template <typename Tag>
    void updateIndexBuffer(
        const GpuBuffer<Tag, GL_ELEMENT_ARRAY_BUFFER, BindingMode::Regular>& indexBuffer);

    /**
     * @brief Updates index type from index buffer.
     * @param indexType The index type.
     * @throws graphics_error If no index buffer is bounded
     */
    void updateIndexType(index_type indexType);

    /**
     * @brief Applies a set of attribute formats and enables them on this VAO.
     * @param attributes Attribute descriptions (index, size, type, offset, binding).
     */
    void setAttributes(std::span<const backend::VertexArrayAttributes> attributes) const;

    /**
     * @brief Sets the instance divisor for a buffer binding.
     *
     * Controls how frequently vertex attributes advance when drawing instanced
     * geometry. A divisor of 0 advances the attributes per vertex, while a
     * divisor of 1 advances them once per instance.
     *
     * @param binding The index of the buffer binding.
     * @param divisor The number of instances between attribute advances.
     */
    void setBindingDivisor(GLuint binding, GLuint divisor) const;

    /**
     * @brief Unbinds any VAO (binds 0) on the current context.
     */
    static void unbind();

    /**
     * @brief Deletes the owned VAO (if any) and resets the handle.
     */
    void release();

    /**
     * @brief Reports whether this VAO is currently bound.
     * @return true if this VAO's handle matches the currently bound VAO.
     */
    [[nodiscard]] bool isBound() const;

    /**
     * @brief Reports whether an index buffer has been attached.
     * @return true if indexCount() is greater than zero.
     */
    [[nodiscard]] bool indexed() const { return indexCount() > 0; }

    /**
     * @brief Returns the number of indices recorded from the attached index buffer.
     * @return Element count (0 when not indexed).
     */
    [[nodiscard]] std::size_t indexCount() const;

    /**
     * @brief Returns the index type from the assigned IndexBuffer
     * @return index_type index enum type
     */
    [[nodiscard]] index_type indexType() const { return static_cast<index_type>(m_IndexType); }

  private:
    VertexArrayHandle m_Handle; /**< Typed handle to the owned VAO. */

    std::size_t m_IndexCount{
        0}; /**< Number of indices recorded by setIndexBuffer (0 = non-indexed). */
    GLenum m_IndexType{GL_UNSIGNED_INT}; /**< Element type of the attached index buffer. */
};

template <typename Tag>
void VertexArray::setIndexBuffer(
    const GpuBuffer<Tag, GL_ELEMENT_ARRAY_BUFFER, BindingMode::Regular>& indexBuffer,
    index_type indexType)
{
    if (!m_Handle.valid() || !indexBuffer.valid())
        throw graphics_error(graphics_error_code::invalid_index_buffer,
                             "A GLHandle is invalid and could not bind to IndexBuffer!");

    // ReSharper disable once CppDFAConstantConditions
    if (indexBuffer.size() <= 0)
        return; // Don't bind invalid indices.

    glVertexArrayElementBuffer(m_Handle.get(), indexBuffer.id());
    m_IndexCount = indexBuffer.size();
    m_IndexType = to_underlying(indexType);
}
template <typename Tag>
void VertexArray::updateIndexBuffer(
    const GpuBuffer<Tag, GL_ELEMENT_ARRAY_BUFFER, BindingMode::Regular>& indexBuffer)
{
    m_IndexCount = indexBuffer.size();
}
} // namespace sog::gfx
