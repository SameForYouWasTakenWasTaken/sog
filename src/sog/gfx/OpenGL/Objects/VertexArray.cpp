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
void VertexArray::setVertexBuffer(const VertexBuffer& buffer, GLuint binding, GLsizei stride)
{
    glVertexArrayVertexBuffer(m_Handle.get(), binding, buffer.id(), 0, stride);
}
void VertexArray::setIndexBuffer(const IndexBuffer& buffer)
{
    glVertexArrayElementBuffer(m_Handle.get(), buffer.id());
    m_IndexCount = buffer.size();
    m_IndexType = GL_UNSIGNED_INT;
}
bool VertexArray::isBound() const
{
    GLint current{};

    glGetIntegerv(to_underlying(vertex_query::BINDING), &current);

    return current != 0 && static_cast<GLuint>(current) == m_Handle.get();
}
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

void VertexArray::unbind() { glBindVertexArray(0); }
void VertexArray::release()
{
    if (m_Handle.valid())
        glDeleteVertexArrays(1, &m_Handle.raw());
    m_Handle.raw() = 0;
}

} // namespace sog::gfx