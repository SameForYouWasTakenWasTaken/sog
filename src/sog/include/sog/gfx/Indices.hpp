/**
 * @file Indices.hpp
 * @brief Defines Index, the unsigned integer type used for element (index) buffer indices.
 */

#pragma once
#include <cstdint>

namespace sog::gfx
{
/// Element index type for index buffers; 32-bit so meshes may exceed 65k vertices.
using Index = std::uint32_t;
} // namespace sog::gfx