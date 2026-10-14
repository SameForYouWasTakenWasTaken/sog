#pragma once
#include "Input.hpp"
#include "Utility/Math.hpp"
#include "Utility/Time.hpp"
#include "Utility/Uniforms.hpp"
#include "gfx/Camera.hpp"
#include "gfx/OpenGL/Objects/Buffers/Buffers.hpp"
#include "gfx/OpenGL/Objects/FrameBuffer.hpp"
#include "gfx/OpenGL/Objects/Shader.hpp"
#include "gfx/OpenGL/Objects/ShaderProgram.hpp"
#include "gfx/OpenGL/Objects/Texture2D.hpp"
#include "gfx/OpenGL/Objects/VertexArray.hpp"
#include "gfx/OpenGL/RenderBuffer.hpp"
#include "gfx/Types/Enums.hpp"
#include "gfx/Window/Window.hpp"

namespace sog
{
using Vertex = gfx::Vertex;
using Index = gfx::Index;

using VertexArray = gfx::VertexArray;
using VertexBuffer = gfx::VertexBuffer;
using IndexBuffer = gfx::IndexBuffer;
template <typename T> using UniformBuffer = gfx::UniformBuffer<T>;
template <typename T> using StorageBuffer = gfx::StorageBuffer<T>;

using VertexShader = gfx::VertexShader;
using FragmentShader = gfx::FragmentShader;
using ShaderProgram = gfx::ShaderProgram;
template <gfx::shader_type TShader> using Shader = gfx::Shader<TShader>;

using Uniform = gfx::Uniform;

using Texture2D = gfx::Texture2D;

using FrameBuffer = gfx::FrameBuffer;
using RenderBuffer = gfx::RenderBuffer;

using Window = gfx::Window;
using ClearBuffer = gfx::ClearBuffer;
using Event = gfx::Event;
using EventType = gfx::EventType;

using Color = gfx::Color;

using Camera = gfx::Camera;

// namespaces
namespace uniform
{
using namespace gfx::uniform;
}

} // namespace sog