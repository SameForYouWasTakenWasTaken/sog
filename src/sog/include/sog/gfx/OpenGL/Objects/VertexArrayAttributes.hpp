/**
 * @file VertexArrayAttributes.hpp
 * @brief Defines backend::VertexArrayAttributes, a plain-data description of one vertex attribute layout (index, size, type, stride, offset, binding).
 *
 * This is the data the engine passes to glVertexAttribPointer-style setup; it
 * carries no GL state of its own.
 */

#pragma once
#include "../../Types/GFXTypes.hpp"

namespace sog::gfx::backend
{
/**
 * @brief Describes the layout of a single vertex attribute within a vertex buffer.
 *
 * Mirrors the arguments of glVertexAttribPointer()/glVertexAttribIPointer():
 * which attribute slot (index), how many components (size), their component
 * type (type), whether integers are normalized (normalized), the byte step
 * between vertices (stride), the byte offset of the first element (offset) and
 * the buffer binding point the attribute reads from (binding).
 *
 * @see sog::Vertex::attributes() for the engine's canonical attribute set
 */
struct VertexArrayAttributes
{
    attribute_index index;        /**< Attribute location in the shader program. */
    attribute_size size;          /**< Component count of this attribute (1-4). */
    attribute_type type;          /**< Component type (e.g. attribute_type::FLOAT32). */
    attribute_normalized normalized; /**< Whether integer data is mapped to [0, 1] (GL_TRUE/GL_FALSE). */
    attribute_stride stride;      /**< Byte distance between consecutive vertices (0 tightly packs). */
    attribute_offset offset;      /**< Byte offset of this attribute within the vertex buffer. */
    attribute_binding binding;    /**< Vertex buffer binding point supplying the data. */

    /**
     * @brief Constructs an attribute description from its layout parameters.
     * @param index Attribute location in the shader program.
     * @param size Component count (1-4).
     * @param type Component type.
     * @param normalized Whether integer components are normalized to [0, 1].
     * @param stride Byte step between consecutive vertices.
     * @param offset Byte offset of the first element within the buffer.
     * @param binding Vertex buffer binding point the attribute reads from.
     */
    constexpr VertexArrayAttributes(attribute_index index, attribute_size size, attribute_type type,
                                    attribute_normalized normalized, attribute_stride stride,
                                    attribute_offset offset, attribute_binding binding);
};

/**
 * @brief Default member-wise constructor: stores each parameter into its like-named field.
 */
constexpr VertexArrayAttributes::VertexArrayAttributes(attribute_index index, attribute_size size,
                                                       attribute_type type,
                                                       attribute_normalized normalized,
                                                       attribute_stride stride,
                                                       attribute_offset offset,
                                                       attribute_binding binding)
    : index(index), size(size), type(type), normalized(normalized), stride(stride), offset(offset),
      binding(binding)
{
}
} // namespace sog::gfx::backend
