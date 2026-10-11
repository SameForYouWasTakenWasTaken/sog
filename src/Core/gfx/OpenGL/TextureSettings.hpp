#pragma once
#include "gfx/Types/Enums.hpp"
namespace sog::gfx
{
struct TextureSettings
{
    texture_params wrapS = texture_params::REPEAT;
    texture_params wrapT = texture_params::REPEAT;

    texture_min_filters minFilter = texture_min_filters::LINEAR;
    texture_mag_filters magFilter = texture_mag_filters::LINEAR;

    bool generateMipmaps = false;
    texture_mipmaps mipmapFilter = texture_mipmaps::LINEAR_LINEAR;
};
} // namespace sog::gfx