#pragma once
#include "../Vertex.hpp"
#include "GpuBuffer.hpp"
#include "gfx/Indices.hpp"

namespace sog::gfx
{
using VertexBuffer = GpuBuffer<Vertex>;
using IndexBuffer = GpuBuffer<Index>;
} // namespace sog::gfx