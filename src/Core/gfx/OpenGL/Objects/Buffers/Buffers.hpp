#pragma once
#include <cstddef>

#include "../../../Vertex.hpp"
#include "GpuBuffer.hpp"
#include "gfx/Indices.hpp"
namespace sog::gfx
{
using VertexBuffer = GpuBuffer<Vertex, GL_ARRAY_BUFFER, BindingMode::Regular>;
using IndexBuffer = GpuBuffer<Index, GL_ELEMENT_ARRAY_BUFFER, BindingMode::Regular>;

template <typename T> using UniformBuffer = GpuBuffer<T, GL_UNIFORM_BUFFER, BindingMode::Indexed>;
template <typename T>
using StorageBuffer = GpuBuffer<T, GL_SHADER_STORAGE_BUFFER, BindingMode::Indexed>;
} // namespace sog::gfx