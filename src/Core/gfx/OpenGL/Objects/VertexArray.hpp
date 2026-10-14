/**
 * @file VertexArray.hpp
 * @brief Defines VertexArray, a RAII wrapper around a GL vertex array object that binds vertex/index buffers and configures attribute formats.
 */

#pragma once
#include <span>
#include <vector>

#include "../../Types/GFXTypes.hpp"
#include "../../Types/Handles.hpp"
#include "../../Vertex.hpp"
#include "Buffers/Buffers.hpp"
#include "VertexArrayAttributes.hpp"
#include "glad/gl.h"

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
    VertexArray(VertexArray&& o) noexcept : m_Handle(std::move(o.m_Handle)) {}

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
     * @brief Attaches a vertex buffer to a binding point of this VAO.
     * @param buffer Vertex buffer supplying the data.
     * @param binding Binding point index to attach to.
     * @param stride Byte distance between consecutive vertices.
     */
    void setVertexBuffer(const VertexBuffer& buffer, GLuint binding, GLsizei stride);

    /**
     * @brief Attaches an index buffer to this VAO and records its element count.
     * @param buffer Index buffer holding the element indices (must not be empty).
     */
    void setIndexBuffer(const IndexBuffer& buffer);

    /**
     * @brief Applies a set of attribute formats and enables them on this VAO.
     * @param attributes Attribute descriptions (index, size, type, offset, binding).
     */
    void setAttributes(std::span<const backend::VertexArrayAttributes> attributes) const;

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
    [[nodiscard]] bool indexed() const { return m_IndexCount > 0; }

    /**
     * @brief Returns the number of indices recorded from the attached index buffer.
     * @return Element count (0 when not indexed).
     */
    [[nodiscard]] std::size_t indexCount() const { return m_IndexCount; }

  private:
    VertexArrayHandle m_Handle; /**< Typed handle to the owned VAO. */
    std::size_t m_IndexCount{0}; /**< Number of indices recorded by setIndexBuffer (0 = non-indexed). */
    GLenum m_IndexType{GL_UNSIGNED_INT}; /**< Element type of the attached index buffer. */
};
} // namespace sog::gfx
