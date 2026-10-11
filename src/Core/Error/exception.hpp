#pragma once
#include <exception>
#include <source_location>
#include <string>

namespace sog
{
class exception : public std::exception
{

  public:
    explicit exception(std::string_view message,
                       std::source_location location = std::source_location::current())
        : m_Message(message), m_Location(location)
    {
    }
    [[nodiscard]]
    const char* what() const noexcept override
    {
        return m_Message.data();
    }

    const std::source_location& location() const noexcept { return m_Location; }
    const std::string_view& message() const noexcept { return m_Message; }

  private:
    std::string_view m_Message;
    std::source_location m_Location;
};
} // namespace sog
