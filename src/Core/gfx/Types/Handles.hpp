#pragma once
#include "../GLHandle.hpp"
#include "gfx/Indices.hpp"
#include "gfx/Vertex.hpp"

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
struct BufferTag
{
};
struct FrameBufferTag
{
};
struct RenderBufferTag
{
};

using VertexArrayHandle = GLHandle<VertexArrayTag>;
using VertexBufferHandle = GLHandle<Vertex>;
using ElementBufferHandle = GLHandle<Index>;

template <typename T> using UniformBufferHandle = GLHandle<T>;
template <typename T> using StorageBufferHandle = GLHandle<T>;

using ShaderHandle = GLHandle<ShaderTag>;
using TextureHandle = GLHandle<TextureTag>;
using FrameBufferHandle = GLHandle<FrameBufferTag>;
using RenderBufferHandle = GLHandle<RenderBufferTag>;

} // namespace sog::gfx