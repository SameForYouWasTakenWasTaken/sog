/**
 * @file Buffers.hpp
 * @brief Defines the engine's concrete GPU buffer aliases (VertexBuffer, IndexBuffer,
 * UniformBuffer, StorageBuffer) as GpuBuffer specializations.
 *
 * Each alias fixes the GL target and binding mode for a buffer role, so callers
 * get the right bind() overload and type safety by construction.
 */

#pragma once
#include <cstddef>
#include <sog/gfx/Indices.hpp>
#include <sog/gfx/OpenGL/Objects/Buffers/GpuBuffer.hpp>
#include <sog/gfx/Types/Handles.hpp>
#include <sog/gfx/Vertex.hpp>
namespace sog::gfx
{
/// Vertex data buffer: bound to GL_ARRAY_BUFFER with regular (target) binding.
using VertexBuffer = GpuBuffer<VertexBufferHandle::tag_type, GL_ARRAY_BUFFER, BindingMode::Regular>;

/// Index buffer for indexed drawing: bound to GL_ELEMENT_ARRAY_BUFFER with regular binding.
using IndexBuffer =
    GpuBuffer<ElementBufferHandle::tag_type, GL_ELEMENT_ARRAY_BUFFER, BindingMode::Regular>;

/**
 * @brief Uniform buffer object for shader uniform blocks, bound to an indexed binding point.
 * @tparam T Element type stored in the block (trivially copyable).
 */
template <typename T>
using UniformBuffer =
    GpuBuffer<typename UniformBufferHandle<T>::tag_type, GL_UNIFORM_BUFFER, BindingMode::Indexed>;

/**
 * @brief Shader storage buffer object (GL 4.3+), bound to an indexed binding point.
 * @tparam T Element type stored in the buffer (trivially copyable).
 */
template <typename T>
using StorageBuffer = GpuBuffer<typename StorageBufferHandle<T>::tag_type, GL_SHADER_STORAGE_BUFFER,
                                BindingMode::Indexed>;
} // namespace sog::gfx
