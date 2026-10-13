#pragma once
#include <span>
#include <vector>

#include "../../Types/GFXTypes.hpp"
#include "../../Types/Handles.hpp"
#include "../../Vertex.hpp"
#include "../../backend/VertexArrayAttributes.hpp"
#include "Buffers/Buffers.hpp"
#include "glad/gl.h"

namespace sog::gfx
{

class VertexArray
{
  public:
    VertexArray();
    ~VertexArray();

    VertexArray(VertexArray&& o) noexcept : m_Handle(std::move(o.m_Handle)) {}
    auto& operator=(VertexArray&& o) noexcept
    {
        if (this != &o)
        {
            release();
            m_Handle = std::move(o.m_Handle);
        }
        return *this;
    }

    void create();
    void bind() const;
    void setVertexBuffer(const VertexBuffer& buffer, GLuint binding, GLsizei stride);
    void setIndexBuffer(const IndexBuffer& buffer);
    void setAttributes(std::span<const backend::VertexArrayAttributes> attributes) const;
    static void unbind();

    void release();

    [[nodiscard]] bool isBound() const;
    [[nodiscard]] bool indexed() const { return m_IndexCount > 0; }
    [[nodiscard]] std::size_t indexCount() const { return m_IndexCount; }

  private:
    VertexArrayHandle m_Handle;
    std::size_t m_IndexCount{0};
    GLenum m_IndexType{GL_UNSIGNED_INT};
};
} // namespace sog::gfx