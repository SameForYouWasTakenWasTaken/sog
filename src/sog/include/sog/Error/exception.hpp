/**
 * @file exception.hpp
 * @brief Defines sog::exception, the base exception type of the engine, capturing a message and the
 * throw-site source location.
 */

#pragma once
#include <exception>
#include <source_location>
#include <string>
#include <utility>

namespace sog
{
/**
 * @brief Base class for all exceptions thrown by the sog engine.
 *
 * Extends std::exception with a descriptive message and the std::source_location
 * where the exception was created, so failures can be reported with file, line and
 * function without the caller threading that information manually. All engine
 * exception types (sog::runtime_error, sog::graphics_error, ...) derive from this.
 *
 * The message is stored by value (an owning std::string), so the exception is
 * fully self-contained: the backing string may safely be a temporary.
 *
 * @see sog::runtime_error, sog::graphics_error
 */
class exception : public std::exception
{

  public:
    /**
     * @brief Constructs an exception with a message and capture location.
     * @param message Human-readable description of the failure (copied into the exception).
     * @param location Capture point of the throw; defaults to the construction site.
     */
    explicit exception(std::string message,
                       std::source_location location = std::source_location::current())
        : m_Message(std::move(message)), m_Location(location)
    {
    }

    /**
     * @brief Returns the message as a C-style string (std::exception interface).
     * @return NUL-terminated message text; valid for the lifetime of the exception.
     */
    [[nodiscard]]
    const char* what() const noexcept override
    {
        return m_Message.c_str();
    }

    /**
     * @brief Returns where in the source this exception was created.
     * @return Copy of the std::source_location captured at construction.
     */
    const std::source_location& location() const noexcept { return m_Location; }

    /**
     * @brief Returns the message.
     * @return The message passed at construction (an owning std::string).
     */
    const std::string& message() const noexcept { return m_Message; }

  private:
    std::string m_Message;           /**< Owning copy of the failure description. */
    std::source_location m_Location; /**< Throw-site capture (file, line, function). */
};
} // namespace sog
