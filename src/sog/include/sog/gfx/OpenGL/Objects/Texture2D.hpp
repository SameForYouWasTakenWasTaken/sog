/**
 * @file Texture2D.hpp
 * @brief Defines Texture2D, a RAII 2D texture that loads from file (via stb_image) or raw pixel
 * data, with configurable sampling settings.
 */

#pragma once
#include <filesystem>
#include <sog/gfx/OpenGL/Objects/TextureSettings.hpp>
#include <sog/gfx/Types/Handles.hpp>

// NOTE: STB_IMAGE_IMPLEMENTATION is defined here so the stb_image implementation
// is compiled exactly once, in the translation unit that includes this header's
// counterpart Texture2D.cpp. See stb_image.h's documentation for the pattern.
#define STB_IMAGE_IMPLEMENTATION

namespace sog::gfx
{
/**
 * @brief Owns a GL 2D texture and manages its creation, upload and binding.
 *
 * createFromFile() decodes an image with stb_image (flipping vertically to match
 * GL's bottom-left origin), converting everything to RGBA8, then uploads it via
 * createFromRaw(). createFromRaw() allocates immutable storage (DSA
 * glTextureStorage2D), uploads the pixel data if provided, applies the wrap and
 * filter settings from TextureSettings, and generates mipmaps if requested.
 *
 * empty() is a convenience factory for an uninitialized texture of a given size
 * (e.g. a render target), and bind() binds the texture to a sampler unit using
 * glBindTextureUnit (DSA).
 *
 * @attention Mipmap generation only happens when both generateMipmaps is set AND
 * pixel data was supplied; allocating storage alone (data == nullptr) leaves the
 * chain ungenerated and sampling may look wrong if a mip filter is selected.
 *
 * @attention The GL texture object is currently never deleted: Texture2D has no
 * destructor, so textures live until the GL context is destroyed.
 *
 * @see sog::TextureSettings
 */
class Texture2D
{
  public:
    /// Constructs an empty Texture2D with no GL object allocated.
    Texture2D() { create(); }

    /**
     * @brief Constructs a texture and loads it from an image file immediately.
     * @param path Path to the image file (any stb_image-supported format).
     * @param settings Sampling settings (wrap, filter, mipmap) to apply.
     * @throws sog::graphics_error With graphics_error_code::texture_load_failed if decoding fails.
     */
    Texture2D(const std::filesystem::path& path, const TextureSettings& settings)
    {
        create();
        createFromFile(path, settings);
    }

    /**
     * @brief Constructs a texture and uploads caller-supplied pixel data immediately.
     * @param data Tightly packed pixel data, or nullptr to allocate storage without uploading.
     * @param width Width in pixels (must be > 0).
     * @param height Height in pixels (must be > 0).
     * @param internalFormat GL internal format to allocate for the storage.
     * @param format GL format of the supplied data.
     * @param type GL data type of the supplied data.
     * @param settings Sampling settings to apply.
     * @throws sog::graphics_error With graphics_error_code::texture_load_failed if width or height
     * is not positive.
     */
    Texture2D(const void* data, int width, int height, texture_internal_format internalFormat,
              pixel_format format, pixel_type type, const TextureSettings& settings)
    {
        create();
        createFromRaw(data, width, height, internalFormat, format, type, settings);
    }

    /**
     * @brief Creates the GL texture object (no-op if already created).
     */
    void create();

    /**
     * @brief Binds this texture to a sampler unit (glBindTextureUnit).
     * @param binding Texture unit index to bind to (e.g. 0).
     * @throws sog::graphics_error With graphics_error_code::texture_not_bound if no GL texture
     * exists.
     */
    void bind(int binding) const;

    /**
     * @brief Loads an image file and creates the texture from its contents.
     * @param path Path to the image file (any stb_image-supported format).
     * @param settings Sampling settings (wrap, filter, mipmap) to apply.
     * @throws sog::graphics_error With graphics_error_code::texture_load_failed if decoding fails.
     *
     * @note The image is flipped vertically on load so it matches OpenGL's
     * bottom-left texture origin, and is always converted to RGBA8.
     */
    void createFromFile(const std::filesystem::path& path, const TextureSettings& settings) const;

    /**
     * @brief Creates the texture from caller-supplied raw pixel data (or allocates storage only).
     * @param data Pointer to tightly packed pixel data, or nullptr to allocate without uploading.
     * @param width Width in pixels (must be > 0).
     * @param height Height in pixels (must be > 0).
     * @param internalFormat GL internal format to allocate (e.g. GL_RGBA8).
     * @param format GL format of the supplied data (e.g. GL_RGBA).
     * @param type GL data type of the supplied data (e.g. GL_UNSIGNED_BYTE).
     * @param settings Sampling settings to apply.
     * @throws sog::graphics_error With graphics_error_code::texture_load_failed if width or height
     * is not positive.
     */
    void createFromRaw(const void* data, int width, int height,
                       texture_internal_format internalFormat, pixel_format format, pixel_type type,
                       const TextureSettings& settings) const;

    /**
     * @brief Returns a reference to the typed GL handle for this texture.
     * @return Const reference to the internal TextureHandle.
     */
    [[nodiscard]] const TextureHandle& getHandle() const { return m_Handle; }

    /**
     * @brief Factory for an allocated-but-uninitialized RGBA8 texture of the given size.
     * @param width Width in pixels.
     * @param height Height in pixels.
     * @return Texture2D with storage allocated and no pixel data uploaded (suitable as a render
     * target).
     */
    static Texture2D empty(int width, int height)
    {
        return Texture2D{nullptr,
                         width,
                         height,
                         texture_internal_format::RGBA8,
                         pixel_format::RGBA,
                         pixel_type::UnsignedByte,
                         {}};
    }

  private:
    TextureHandle m_Handle; /**< Typed handle to the owned GL texture. */
};
} // namespace sog::gfx
