#pragma once
#include "../../Types/GFXTypes.hpp"

namespace sog::gfx::backend
{
struct VertexArrayAttributes
{
    attribute_index index;
    attribute_size size;
    attribute_type type;
    attribute_normalized normalized;
    attribute_stride stride;
    attribute_offset offset;
    attribute_binding binding;

    constexpr VertexArrayAttributes(attribute_index index, attribute_size size, attribute_type type,
                                    attribute_normalized normalized, attribute_stride stride,
                                    attribute_offset offset, attribute_binding binding);
};
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