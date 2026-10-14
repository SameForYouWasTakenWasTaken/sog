/**
 * @file runtime_error.hpp
 * @brief Defines sog::runtime_error, a general-purpose runtime failure in the sog engine.
 */

#pragma once
#include "../exception.hpp"
namespace sog
{
/**
 * @brief General-purpose runtime error for non-graphics failures.
 *
 * Inherits the sog::exception constructors and destructor unchanged; it exists to
 * give non-graphics runtime failures their own catchable type while sharing the
 * message and std::source_location capture behaviour of the base class.
 *
 * @see sog::exception
 */
class runtime_error : public exception
{
  public:
    using exception::exception;      /**< Reuses the sog::exception (message, source_location) constructors. */
    ~runtime_error() override = default; /**< Defaulted virtual destructor (inherited from std::exception). */
};
} // namespace sog
