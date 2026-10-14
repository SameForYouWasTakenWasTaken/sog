# Dependency order: Shared first, then Core, then App.
# Core: engine layer. Links Shared, glm and stb.
add_library(SOG STATIC
        # Core & Utility
        sog/Input.cpp
        sog/Utility/Time.cpp
        sog/Utility/Uniforms.cpp

        # Graphics & Windowing
        sog/gfx/Camera.cpp
        sog/gfx/Window/Window.cpp
        sog/gfx/OpenGL/RenderBuffer.cpp
        sog/gfx/OpenGL/Objects/VertexArray.cpp
        sog/gfx/OpenGL/Objects/Texture2D.cpp
        sog/gfx/OpenGL/Objects/FrameBuffer.cpp
        sog/gfx/OpenGL/Objects/ShaderProgram.cpp

        # GLAD Loader (adjust path to where your glad.c is located)
        sog/vendor/glad/gl.c
        sog/gfx/OpenGL/Common.cpp
)

target_include_directories(SOG
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sog/include>
        $<INSTALL_INTERFACE:include>
        sog/vendor
)

target_link_libraries(SOG
        PUBLIC
        glm::glm
        PRIVATE
        glfw
        stb
)

target_compile_definitions(SOG PRIVATE
        GLFW_INCLUDE_NONE
)

