# SOGTests: GoogleTest suite exercising the SOG gfx layer against real (hidden) OpenGL contexts.
#
# A single test executable links the SOG static library and GTest::gtest. It provides its own
# main() (see main.cpp) instead of gtest_main, because the suite must initialize GLFW up front
# and set GLFW_VISIBLE so every context the tests create stays off-screen.
add_executable(SOGTests
        main.cpp
        TestSupport.cpp

        CameraTests.cpp
        GLHandleTests.cpp
        BufferTests.cpp
        VertexArrayTests.cpp
        ShaderTests.cpp
        ShaderProgramTests.cpp
        UniformTests.cpp
        Texture2DTests.cpp
        FrameBufferTests.cpp
        RenderBufferTests.cpp
        WindowTests.cpp
        CommonDrawTests.cpp
        ColorTests.cpp
        ClearBufferTests.cpp
        EnumsTests.cpp
        GraphicsErrorTests.cpp
        OpenGLDebugTests.cpp
        CameraRotationMatrixTests.cpp
        VertexArrayMoveTests.cpp
        GpuBufferTests.cpp
)

target_link_libraries(SOGTests
        PRIVATE
        SOG
        GTest::gtest
)

# Both the library and the tests use these public headers, so the tests reuse SOG's include path
# (gotten transitively via SOG) plus this directory for TestSupport.hpp.
target_include_directories(SOGTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Absolute path to real shader/texture assets so tests can load from file regardless of the
# working directory CTest picks for a given test.
target_compile_definitions(SOGTests PRIVATE SOG_TEST_ASSET_DIR="${CMAKE_SOURCE_DIR}/src/examples/assets")

# Register each GoogleTest case as its own CTest test, discovered by running the binary once.
include(GoogleTest)
gtest_discover_tests(SOGTests
        DISCOVERY_TIMEOUT 120
        PROPERTIES TIMEOUT 120
)
