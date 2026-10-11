#pragma once
#include <optional>

#include "../OpenGL/ShaderProgram.hpp"
#include "../OpenGL/VertexArray.hpp"
#include "gfx/OpenGL/Texture2D.hpp"

namespace sog::gfx
{
struct RenderState
{
    const VertexBuffer& vertexBuffer;
    const VertexArray& vertexArray;
    const Texture2D* texture2D = nullptr;

    const ShaderProgram& shaderProgram;
};
} // namespace sog::gfx
