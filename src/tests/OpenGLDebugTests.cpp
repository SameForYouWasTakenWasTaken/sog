/**
 * @file OpenGLDebugTests.cpp
 * @brief Exercises the sog::debug debug-output bridge: setDebugCallback() translation of GL debug
 * messages into human-readable source/type/severity strings, callback replacement and clearing.
 *
 * The suite's context is a debug context (Window::initialize requests GLFW_OPENGL_DEBUG_CONTEXT),
 * so an invalid GLenum raised with glEnable() reliably produces a synchronous API/ERROR/HIGH
 * debug message (id 1280 = GL_INVALID_ENUM).
 */

#include <TestSupport.hpp>
#include <sog/gfx/backend/OpenGLDebug.hpp>

#include <string>

using sogtest::GLTest;

namespace
{
/// Everything the debug bridge hands to a user callback, captured for assertions.
struct CapturedMessage
{
    bool called = false;
    std::string source;
    std::string type;
    std::string severity;
    GLuint id = 0;
    std::string message;
};

/// Builds a callback that records the next message into `sink`.
sog::debug::DebugCallback recorder(CapturedMessage& sink)
{
    return [&sink](const char* source, const char* type, const char* severity, GLuint id,
                   const char* message)
    {
        sink.called = true;
        sink.source = source;
        sink.type = type;
        sink.severity = severity;
        sink.id = id;
        sink.message = message;
    };
}

/// Fixture guaranteeing the registered callback is detached before the test's locals die.
class DebugOutputTest : public GLTest
{
  protected:
    void TearDown() override
    {
        // The captured callback references test-body locals: clear it first.
        sog::debug::setDebugCallback(nullptr);
        GLTest::TearDown();
    }
};

/// Raises GL_INVALID_ENUM through an unknown enum, generating a debug message.
void raiseGlError() { glEnable(static_cast<GLenum>(0xDEAD)); }
} // namespace

TEST_F(DebugOutputTest, ErrorMessageIsTranslatedAndDelivered)
{
    CapturedMessage captured;
    sog::debug::setDebugCallback(recorder(captured));

    raiseGlError();

    // Delivery is synchronous (GL_DEBUG_OUTPUT_SYNCHRONOUS is enabled by setDebugCallback), so the
    // message must already have arrived before any other GL work happens.
    ASSERT_TRUE(captured.called) << "the debug callback must run synchronously";
    EXPECT_EQ(captured.source, "API");
    EXPECT_EQ(captured.type, "ERROR");
    EXPECT_EQ(captured.severity, "HIGH");
    // The message id of an API-sourced error is driver-defined (this driver reports 1002), so
    // verify the actual GL_INVALID_ENUM through the error queue instead of pinning the id.
    EXPECT_EQ(glGetError(), GL_INVALID_ENUM) << "glEnable(unknown enum) must raise GL_INVALID_ENUM";
    EXPECT_NE(captured.id, 0u) << "the bridge must forward a real message id";
    EXPECT_FALSE(captured.message.empty());

    drainGLErrors();
}

TEST_F(DebugOutputTest, CallbackReceivesTheDriverMessageText)
{
    CapturedMessage captured;
    sog::debug::setDebugCallback(recorder(captured));

    raiseGlError();

    ASSERT_TRUE(captured.called);
    // NVIDIA/AMD/Intel word this differently, but an enum-related complaint must mention something.
    EXPECT_GT(captured.message.size(), 8u) << "driver message should be a sentence, not a token";

    drainGLErrors();
}

TEST_F(DebugOutputTest, RegisteringANewCallbackReplacesTheOldOne)
{
    CapturedMessage first;
    CapturedMessage second;
    sog::debug::setDebugCallback(recorder(first));
    sog::debug::setDebugCallback(recorder(second));

    raiseGlError();

    EXPECT_TRUE(second.called);
    EXPECT_FALSE(first.called) << "the replaced callback must not fire any more";

    drainGLErrors();
}

TEST_F(DebugOutputTest, ClearingTheCallbackStopsDelivery)
{
    CapturedMessage captured;
    sog::debug::setDebugCallback(recorder(captured));
    sog::debug::setDebugCallback(nullptr);

    raiseGlError();

    EXPECT_FALSE(captured.called) << "no user callback means no delivery";

    drainGLErrors();
}

TEST_F(DebugOutputTest, DebugOutputStateIsEnabled)
{
    sog::debug::setDebugCallback(nullptr);
    drainGLErrors();

    EXPECT_TRUE(glIsEnabled(GL_DEBUG_OUTPUT));
    EXPECT_TRUE(glIsEnabled(GL_DEBUG_OUTPUT_SYNCHRONOUS));
    expectNoGLError();
}
