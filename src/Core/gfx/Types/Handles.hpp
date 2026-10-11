#pragma once
#include "../GLHandle.hpp"

namespace sog::gfx
{
struct VertexBufferTag
{
};
struct VertexArrayTag
{
};
struct ElementBufferTag
{
};
struct ShaderTag
{
};

struct TextureTag
{
};

using VertexArrayHandle = GLHandle<VertexArrayTag>;
using VertexBufferHandle = GLHandle<VertexBufferTag>;
using ElementBufferHandle = GLHandle<ElementBufferTag>;
using ShaderHandle = GLHandle<ShaderTag>;
using TextureHandle = GLHandle<TextureTag>;

} // namespace sog::gfx