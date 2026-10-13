#pragma once
#include <filesystem>

#include "../../Types/Handles.hpp"
#include "TextureSettings.hpp"

#define STB_IMAGE_IMPLEMENTATION

namespace sog::gfx
{
class Texture2D
{
  public:
    Texture2D() = default;

    void create();
    void bind(int binding) const;
    void createFromFile(const std::filesystem::path& path, const TextureSettings& settings);
    void createFromRaw(const void* data, int width, int height, GLenum internalFormat,
                       GLenum format, GLenum type, const TextureSettings& settings);

    [[nodiscard]] const TextureHandle& getHandle() const { return m_Handle; }

    static Texture2D empty(int width, int height)
    {
        Texture2D t;
        t.createFromRaw(nullptr, width, height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, {});
        return std::move(t);
    }

  private:
    TextureHandle m_Handle;
};
} // namespace sog::gfx