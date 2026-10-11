#pragma once
#include <filesystem>

#include "../Types/Handles.hpp"
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

  private:
    TextureHandle m_Handle;
};
} // namespace sog::gfx