/**
 * @file Vertex.hpp
 * @brief Defines Vertex, the engine's interleaved vertex layout (position + color + texture coordinate) with its matching attribute descriptions.
 */

#pragma once
#include "OpenGL/Objects/VertexArrayAttributes.hpp"
#include "Types/Math.hpp"

namespace sog::gfx
{
/**
 * @brief A single vertex: 3-float position, 4-float color, 2-float texture coordinate.
 *
 * Layout is fixed and interleaved in this member order, so sizeof(Vertex) is the
 * stride for every attribute. attributes() returns the matching
 * backend::VertexArrayAttributes array (positions at location 0, colors at 1,
 * texcoords at 2) for configuring a vertex array object against a shader program
 * that uses the same locations.
 *
 * @see sog::backend::VertexArrayAttributes, sog::VertexArray
 */
struct Vertex
{

    math::Vec3 position = {}; /**< Vertex position (x, y, z). */
    math::Vec4 color = {};    /**< Per-vertex RGBA color. */
    math::Vec2 texCoord = {}; /**< Texture coordinate (u, v). */

    static constexpr GLuint PositionIndex = 0;     ///< Shader attribute location for position.
    static constexpr GLuint ColorIndex = 1;        ///< Shader attribute location for color.
    static constexpr GLuint TexCoordinateIndex = 2; ///< Shader attribute location for texture coordinate.

    /**
     * @brief Returns the byte size of one vertex, used as the attribute stride.
     * @return sizeof(Vertex), the byte distance between consecutive vertices.
     */
    static constexpr GLsizei stride() { return sizeof(Vertex); }

    /**
     * @brief Builds the attribute layout describing how to read a Vertex from a vertex buffer.
     * @return std::array of three backend::VertexArrayAttributes (position, color, texCoord)
     * using offsets computed from this struct's memory layout.
     */
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
