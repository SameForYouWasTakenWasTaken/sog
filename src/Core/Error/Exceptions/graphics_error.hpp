#pragma once
#include "../exception.hpp"

namespace sog
{
enum class graphics_error_code
{
    window_creation_failed,
    invalid_operation,
    shader_compilation_failed,
    shader_link_failed,
    buffer_creation_failed,
    vertex_array_creation_failed,
    vertex_array_not_bound,
    buffer_not_bound,
    uniform_location_invalid,

    shader_file_open_failed,

    texture_load_failed,
    texture_not_bound,
};

class graphics_error : public exception
{
  public:
    graphics_error(graphics_error_code code, std::string_view message,
                   std::source_location location = std::source_location::current())
        : exception(message, location), m_code(code)
    {
    }

    [[nodiscard]]
    graphics_error_code code() const noexcept
    {
        return m_code;
    }

  private:
    graphics_error_code m_code;
};
} // namespace sog