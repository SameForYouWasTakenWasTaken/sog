#pragma once
#include <optional>

#include "../OpenGL/Objects/ShaderProgram.hpp"
#include "../OpenGL/Objects/VertexArray.hpp"
#include "gfx/OpenGL/Objects/Texture2D.hpp"

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
