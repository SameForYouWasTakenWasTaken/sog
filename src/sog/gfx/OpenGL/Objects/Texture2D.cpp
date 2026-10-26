/**
 * @file Texture2D.cpp
 * @brief Implements Texture2D: GL texture creation, stb_image file loading, raw upload with
 * immutable storage and sampler settings, and release.
 */

#include <algorithm>
#include <cmath>
#include <fstream>
#include <sog/Error/Exceptions/graphics_error.hpp>
#include <sog/Utility/TypeTraits.hpp>
#include <sog/gfx/OpenGL/Objects/Texture2D.hpp>

#include "glad/gl.h"

// The stb_image implementation is compiled exactly once, in this translation unit.
// See stb_image.h's documentation for the pattern.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace sog::gfx
{

void Texture2D::create()
{
    if (m_Handle.valid())
        return;
    glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle.raw());
}
void Texture2D::bind(int binding) const
{
    if (!m_Handle.valid())
        throw graphics_error(graphics_error_code::texture_not_bound, "Could not bind texture!");
    glBindTextureUnit(binding, m_Handle.get());
}
void Texture2D::createFromFile(const std::filesystem::path& path,
                               const TextureSettings& settings) const
{
    const std::string pathString = path.string();

    int width = 0;
    int height = 0;
    stbi_set_flip_vertically_on_load(true);
    stbi_uc* data = stbi_load(pathString.c_str(), &width, &height, nullptr, STBI_rgb_alpha);

    if (!data)
    {
        throw graphics_error(graphics_error_code::texture_load_failed,
                             "Failed to load texture: " + pathString);
    }

    createFromRaw(data, width, height, texture_internal_format::RGBA8, pixel_format::RGBA,
                  pixel_type::UnsignedByte, settings);

    stbi_image_free(data);
}
void Texture2D::createFromRaw(const void* data, int width, int height,
                              texture_internal_format internalFormat, pixel_format format,
                              pixel_type type, const TextureSettings& settings) const
{
    if (width <= 0 || height <= 0)
    {
        throw graphics_error(graphics_error_code::texture_load_failed,
                             "Invalid texture dimensions for createFromRaw.");
    }

    // Storage is immutable (glTextureStorage2D): it may be allocated only once per texture
    // object, and a second upload would otherwise surface as a raw GL_INVALID_OPERATION with
    // no context. Query the driver's immutable-format flag and fail loudly instead.
    GLint immutableFormat = GL_FALSE;
    glGetTextureParameteriv(m_Handle.get(), GL_TEXTURE_IMMUTABLE_FORMAT, &immutableFormat);
    if (immutableFormat == GL_TRUE)
    {
        throw graphics_error(
            graphics_error_code::invalid_operation,
            "Texture already has immutable storage allocated; "
            "construct a new Texture2D instead of re-creating this one.");
    }

    const GLsizei levels =
        settings.generateMipmaps
            ? static_cast<GLsizei>(std::floor(std::log2(std::max(width, height))) + 1)
            : 1;

    // Use underlying values for OpenGL calls
    glTextureStorage2D(m_Handle.get(), levels, to_underlying(internalFormat), width, height);

    if (data != nullptr)
    {
        glTextureSubImage2D(m_Handle.get(), 0, 0, 0, width, height, to_underlying(format),
                            to_underlying(type), data);
    }

    glTextureParameteri(m_Handle.get(), GL_TEXTURE_WRAP_S, to_underlying(settings.wrapS));
    glTextureParameteri(m_Handle.get(), GL_TEXTURE_WRAP_T, to_underlying(settings.wrapT));
    glTextureParameteri(m_Handle.get(), GL_TEXTURE_MIN_FILTER,
                        to_underlying(settings.minFilter));
    glTextureParameteri(m_Handle.get(), GL_TEXTURE_MAG_FILTER,
                        to_underlying(settings.magFilter));

    if (settings.generateMipmaps && data != nullptr)
    {
        glGenerateTextureMipmap(m_Handle.get());
    }
}
void Texture2D::release()
{
    if (m_Handle.valid())
    {
        glDeleteTextures(1, &m_Handle.raw());
        m_Handle.raw() = 0;
    }
}
} // namespace sog::gfx