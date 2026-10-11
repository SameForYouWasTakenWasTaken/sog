#pragma once
#include "../exception.hpp"
namespace sog
{
class runtime_error : public exception
{
  public:
    using exception::exception;
    ~runtime_error() override = default;
};
} // namespace sog