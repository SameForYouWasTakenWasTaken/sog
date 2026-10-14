# Static library target "SOG": public headers live under sog/include, implementations under
# sog/, and vendored glad under sog/vendor.
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
        sog/gfx/backend/OpenGLDebug.cpp

        # GLAD Loader
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
        # glfw is public: sog::input exposes the Key enum (GLFW key codes) and its header
        # includes GLFW/glfw3.h, so consumers need glfw's headers on their include path.
        glfw
        PRIVATE
        stb
)

target_compile_definitions(SOG PUBLIC
        # Keep glfw3.h from including system GL headers: glad (vendored, included by the public
        # GLHandle.hpp) is the sole GL entry-point provider.
        GLFW_INCLUDE_NONE
)

