/**
 * @file GraphicsErrorTests.cpp
 * @brief Exercises the graphics error/exception types: graphics_error, graphics_error_code and the
 * sog::exception base.
 */

#include <TestSupport.hpp>

#include <stdexcept>
#include <string>
#include <type_traits>

using sog::exception;
using sog::graphics_error;
using sog::graphics_error_code;

TEST(GraphicsError, CarriesCodeAndMessage)
{
    const graphics_error error{graphics_error_code::shader_compilation_failed, "compile failed"};

    EXPECT_EQ(error.code(), graphics_error_code::shader_compilation_failed);
    EXPECT_EQ(error.message(), "compile failed");
}

TEST(GraphicsError, WhatReturnsMessage)
{
    const graphics_error error{graphics_error_code::texture_load_failed, "texture missing"};

    EXPECT_STREQ(error.what(), "texture missing");
}

TEST(GraphicsError, CapturesSourceLocation)
{
    const graphics_error error{graphics_error_code::window_creation_failed, "no window"};

    const auto& location = error.location();
    EXPECT_NE(location.file_name(), nullptr);
    EXPECT_GT(location.line(), 0u);
}

TEST(GraphicsError, IsCatchableAsStdException)
{
    try
    {
        throw graphics_error{graphics_error_code::invalid_operation, "bad op"};
    }
    catch (const std::exception& caught)
    {
        EXPECT_STREQ(caught.what(), "bad op");
        SUCCEED();
        return;
    }

    FAIL() << "graphics_error must be catchable as std::exception";
}

TEST(GraphicsError, IsCatchableAsGraphicsError)
{
    try
    {
        throw graphics_error{graphics_error_code::buffer_creation_failed, "no buffer"};
    }
    catch (const graphics_error& caught)
    {
        EXPECT_EQ(caught.code(), graphics_error_code::buffer_creation_failed);
        SUCCEED();
        return;
    }

    FAIL() << "graphics_error must be catchable as graphics_error";
}

TEST(GraphicsError, DerivesFromExceptionBase)
{
    static_assert(std::is_base_of_v<exception, graphics_error>,
                  "graphics_error must derive from sog::exception");
    SUCCEED();
}

TEST(ExceptionBase, CarriesMessageAndLocation)
{
    const exception base{"something broke"};

    EXPECT_STREQ(base.what(), "something broke");
    EXPECT_EQ(base.message(), "something broke");
    EXPECT_GT(base.location().line(), 0u);
}

TEST(ExceptionBase, WhatIsNoexcept)
{
    // The std::exception interface requires what() to be noexcept; verify the override honors it.
    const exception base{"msg"};
    static_assert(noexcept(base.what()), "exception::what() must be noexcept");
    SUCCEED();
}
