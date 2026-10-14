/**
 * @file RenderBuffer.cpp
 * @brief Implements RenderBuffer: renderbuffer creation, DSA storage allocation and release.
 */

#include <sog/gfx/OpenGL/RenderBuffer.hpp>

namespace sog::gfx
{

void RenderBuffer::create(GLenum internalFormat, GLsizei width, GLsizei height)
{
    if (!m_Handle.valid())
    {
        glCreateRenderbuffers(1, &m_Handle.raw());
    }
    m_InternalFormat = internalFormat;
    m_Width = width;
    m_Height = height;

    // Allocate storage using DSA (OpenGL 4.5+)
    glNamedRenderbufferStorage(m_Handle.get(), internalFormat, width, height);
}
void RenderBuffer::release()
{
    if (m_Handle.valid())
    {
        glDeleteRenderbuffers(1, &m_Handle.raw());
        m_Handle.raw() = 0;
        m_Width = 0;
        m_Height = 0;
        m_InternalFormat = 0;
    }
}
} // namespace sog::gfx