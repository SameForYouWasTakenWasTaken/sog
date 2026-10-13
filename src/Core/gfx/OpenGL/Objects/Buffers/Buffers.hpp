#pragma once
#include <cstddef>

#include "../../../Vertex.hpp"
#include "GpuBuffer.hpp"
#include "gfx/Indices.hpp"
namespace sog::gfx
{
using VertexBuffer = GpuBuffer<VertexBufferHandle::tag_type, GL_ARRAY_BUFFER, BindingMode::Regular>;
using IndexBuffer =
    GpuBuffer<ElementBufferHandle::tag_type, GL_ELEMENT_ARRAY_BUFFER, BindingMode::Regular>;

template <typename T>
using UniformBuffer =
    GpuBuffer<typename UniformBufferHandle<T>::tag_type, GL_UNIFORM_BUFFER, BindingMode::Indexed>;

template <typename T>
using StorageBuffer = GpuBuffer<typename StorageBufferHandle<T>::tag_type, GL_SHADER_STORAGE_BUFFER,
                                BindingMode::Indexed>;
} // namespace sog::gfx