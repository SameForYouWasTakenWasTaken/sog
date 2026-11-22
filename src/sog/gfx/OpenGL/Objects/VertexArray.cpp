/**
 * @file VertexArray.cpp
 * @brief Implements VertexArray: VAO creation, DSA-based buffer/attribute setup, bind state queries
 * and release.
 */

#include <sog/Error/Exceptions/graphics_error.hpp>
#include <sog/Utility/TypeTraits.hpp>
#include <sog/gfx/OpenGL/Objects/VertexArray.hpp>
#include <sog/gfx/Types/Enums.hpp>

#include "glad/gl.h"

namespace sog::gfx
{

VertexArray::VertexArray() = default;
VertexArray::~VertexArray() { release(); }
void VertexArray::create()
{
    if (m_Handle.valid())
        return;

    glCreateVertexArrays(1, &m_Handle.raw());
}
void VertexArray::bind() const
{
    if (!m_Handle.valid())
        throw graphics_error(graphics_error_code::buffer_creation_failed,
                             "GL Handle is invalid and could not bind VertexArray!");
    glBindVertexArray(m_Handle.get());
}

void VertexArray::setIndexBuffer(const IndexBuffer& buffer, index_type indexType)
{
    if (!m_Handle.valid() || !buffer.valid())
        throw graphics_error(graphics_error_code::invalid_index_buffer,
                             "A GLHandle is invalid and could not bind to IndexBuffer!");

    // ReSharper disable once CppDFAConstantConditions
    if (buffer.size() <= 0)
        return; // Don't bind invalid indices.

    glVertexArrayElementBuffer(m_Handle.get(), buffer.id());
    m_IndexCount = buffer.size();
    m_IndexType = to_underlying(indexType);
}

void VertexArray::updateIndexBuffer(const IndexBuffer& buffer) { m_IndexCount = buffer.size(); }
void VertexArray::updateIndexType(index_type indexType)
{
    if (!indexed())
        throw graphics_error(
            graphics_error_code::invalid_index_buffer,
            "Could not update index type, as no index buffer is bound to the vertex array!");
    m_IndexType = to_underlying(indexType);
}

bool VertexArray::isBound() const
{
    GLint current{};

    glGetIntegerv(to_underlying(vertex_query::BINDING), &current);

    return current != 0 && static_cast<GLuint>(current) == m_Handle.get();
}
std::size_t VertexArray::indexCount() const { return m_IndexCount; }
void VertexArray::setAttributes(std::span<const backend::VertexArrayAttributes> attributes) const
{
    for (const auto& attribute : attributes)
    {
        glVertexArrayAttribFormat(m_Handle.get(), attribute.index, attribute.size,
                                  to_underlying(attribute.type), attribute.normalized,
                                  attribute.offset);

        glVertexArrayAttribBinding(m_Handle.get(), attribute.index, attribute.binding);

        glEnableVertexArrayAttrib(m_Handle.get(), attribute.index);
    }
}
void VertexArray::setBindingDivisor(GLuint binding, GLuint divisor) const
{
    glVertexArrayBindingDivisor(m_Handle.get(), binding, divisor);
}

void VertexArray::unbind() { glBindVertexArray(0); }
void VertexArray::release()
{
    if (m_Handle.valid())
        glDeleteVertexArrays(1, &m_Handle.raw());
    m_Handle.raw() = 0;
}

} // namespace sog::gfx