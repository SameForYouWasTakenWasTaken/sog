#include "Texture2D.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>

#include "Error/Exceptions/graphics_error.hpp"
#include "Util.hpp"
#include "gfx/Types/Enums.hpp"
#include "glad/gl.h"
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
void Texture2D::createFromFile(const std::filesystem::path& path, const TextureSettings& settings)
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
    if (!m_Handle.valid())
        glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle.raw());

    const GLsizei levels =
        settings.generateMipmaps
            ? static_cast<GLsizei>(std::floor(std::log2(std::max(width, height))) + 1)
            : 1;

    glTextureStorage2D(m_Handle.get(), levels, GL_RGBA8, width, height);

    glTextureSubImage2D(m_Handle.get(), 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTextureParameteri(m_Handle.get(), GL_TEXTURE_WRAP_S, util::to_underlying(settings.wrapS));

    glTextureParameteri(m_Handle.get(), GL_TEXTURE_WRAP_T, util::to_underlying(settings.wrapT));

    glTextureParameteri(m_Handle.get(), GL_TEXTURE_MIN_FILTER,
                        util::to_underlying(settings.minFilter));

    glTextureParameteri(m_Handle.get(), GL_TEXTURE_MAG_FILTER,
                        util::to_underlying(settings.magFilter));

    if (settings.generateMipmaps)
        glGenerateTextureMipmap(m_Handle.get());

    stbi_image_free(data);
}
} // namespace sog::gfx