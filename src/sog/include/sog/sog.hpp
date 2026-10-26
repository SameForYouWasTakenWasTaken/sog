/**
 * @file sog.hpp
 * @brief Umbrella header of the sog library: pulls in the public API and re-exports the most
 * commonly used types into the top-level sog:: namespace.
 *
 * Including <sog/sog.hpp> gives a program the whole library surface: windowing
 * (Window, Event, ClearBuffer), drawing (VertexArray, buffers, shaders,
 * Texture2D, FrameBuffer, RenderBuffer, Uniform), the Camera, input queries
 * (Input.hpp) and the Utility helpers (Math, Time, TypeTraits, Uniforms).
 *
 * Types live in sog::gfx (and its inline namespaces); the aliases declared here
 * let typical application code use the short names (sog::Window, sog::Vertex,
 * sog::ShaderProgram, ...) without naming the gfx namespace. The enums and
 * uniform helpers are additionally re-exported as sog::enums and sog::uniform.
 */

#pragma once
#include "Input.hpp"
#include "Utility/Math.hpp"
#include "Utility/Time.hpp"
#include "Utility/TypeTraits.hpp"
#include "Utility/Uniforms.hpp"
#include "gfx/Camera.hpp"
#include "gfx/OpenGL/Common.hpp"
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
/// A single interleaved vertex (position, color, texcoord); alias of gfx::Vertex.
using Vertex = gfx::Vertex;
/// Element index type of index buffers (32-bit unsigned); alias of gfx::Index.
using Index = gfx::Index;
/// Vertex array object wrapper; alias of gfx::VertexArray.
using VertexArray = gfx::VertexArray;
/// Compiled vertex shader stage; alias of gfx::VertexShader.
using VertexShader = gfx::VertexShader;
/// Compiled fragment shader stage; alias of gfx::FragmentShader.
using FragmentShader = gfx::FragmentShader;
/// Linked shader program; alias of gfx::ShaderProgram.
using ShaderProgram = gfx::ShaderProgram;
/// Shader stage of a given shader_type; alias of gfx::Shader.
template <gfx::shader_type TShader> using Shader = gfx::Shader<TShader>;

/// Location wrapper for a shader uniform; alias of gfx::Uniform.
using Uniform = gfx::Uniform;

/// RAII 2D texture; alias of gfx::Texture2D.
using Texture2D = gfx::Texture2D;

/// Framebuffer object for offscreen rendering; alias of gfx::FrameBuffer.
using FrameBuffer = gfx::FrameBuffer;
/// Renderbuffer object (e.g. depth/stencil storage); alias of gfx::RenderBuffer.
using RenderBuffer = gfx::RenderBuffer;

/// GLFW-backed OS window with an OpenGL context; alias of gfx::Window.
using Window = gfx::Window;
/// Bitmask of buffers to clear (color/depth/stencil); alias of gfx::ClearBuffer.
using ClearBuffer = gfx::ClearBuffer;
/// Window event payload; alias of gfx::Event.
using Event = gfx::Event;
/// Discriminator for the payload an Event carries; alias of gfx::EventType.
using EventType = gfx::EventType;

/// RGBA color on the 8-bit scale (channels 0 - 255); alias of gfx::Color.
using Color = gfx::Color;

/// Fly-style camera producing view/projection matrices; alias of gfx::Camera.
using Camera = gfx::Camera;

// namespaces
/// Draw helpers (DrawArrays, DrawElements, ...) from sog::gfx::common.
using namespace gfx::common;
/// Buffers (VertexBuffer, UniformBuffer, ...)
using namespace gfx::buffers;
/// Type-safe GL enums (buffer_target, shader_type, draw_mode, ...) under the sog::enums namespace.
namespace enums
{
using namespace gfx::enums;
}
/// Uniform lookup/upload helpers (getLocation, setVec4, ...) under the sog::uniform namespace.
namespace uniform
{
using namespace gfx::uniform;
}

/// Math utility convenience
using namespace math::types;
namespace math
{
using namespace glm;
}
} // namespace sog