#pragma once
#include "OpenGL/Objects/VertexArrayAttributes.hpp"
#include "Types/Math.hpp"

namespace sog::gfx
{
struct Vertex
{

    math::Vec3 position = {};
    math::Vec4 color = {};
    math::Vec2 texCoord = {};

    static constexpr GLuint PositionIndex = 0;
    static constexpr GLuint ColorIndex = 1;
    static constexpr GLuint TexCoordinateIndex = 2;

    static constexpr GLsizei stride() { return sizeof(Vertex); }

    static constexpr auto attributes()
    {
        return std::array{
            backend::VertexArrayAttributes{PositionIndex, 3, attribute_type::FLOAT32, false,
                                           stride(), offsetof(Vertex, position), 0},
            backend::VertexArrayAttributes{ColorIndex, 4, attribute_type::FLOAT32, false, stride(),
                                           offsetof(Vertex, color), 0},
            backend::VertexArrayAttributes{TexCoordinateIndex, 2, attribute_type::FLOAT32, false,
                                           stride(), offsetof(Vertex, texCoord), 0}};
    }
};
} // namespace sog::gfx