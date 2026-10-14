/**
 * @file TextureSettings.hpp
 * @brief Defines TextureSettings, a plain struct collecting the sampling parameters (wrap modes, filters, mipmap options) applied to a texture.
 */

#pragma once
#include "../../Types/Enums.hpp"
namespace sog::gfx
{
/**
 * @brief Sampling configuration applied to a texture when its storage is created.
 *
 * Groups the wrap modes for each axis, the minification and magnification
 * filters, and the mipmap options. Defaults repeat and use linear filtering with
 * mipmapping disabled.
 *
 * @see sog::Texture2D::createFromFile, sog::Texture2D::createFromRaw
 */
struct TextureSettings
{
    texture_params wrapS = texture_params::REPEAT; /**< Horizontal (S) wrap mode for coordinates outside [0, 1]. */
    texture_params wrapT = texture_params::REPEAT; /**< Vertical (T) wrap mode for coordinates outside [0, 1]. */

    texture_min_filters minFilter = texture_min_filters::LINEAR; /**< Filter used when the texture is minified (shrunk on screen). */
    texture_mag_filters magFilter = texture_mag_filters::LINEAR; /**< Filter used when the texture is magnified (enlarged on screen). */

    bool generateMipmaps = false; /**< Whether to allocate a full mipmap chain and generate it after upload. */
    texture_mipmaps mipmapFilter = texture_mipmaps::LINEAR_LINEAR; /**< How mip levels are selected/interpolated when generateMipmaps is true. */
};
} // namespace sog::gfx
