# Core: engine layer. Links Shared, glm and stb.
add_library(Core STATIC
        Engine.cpp
        gfx/OpenGL/Objects/VertexArray.cpp
        gfx/Window/Window.cpp
        gfx/OpenGL/Uniforms.cpp
        gfx/OpenGL/Objects/Texture2D.cpp
        gfx/Camera.cpp
        Input.cpp
        Utility/Time.cpp
        gfx/OpenGL/Objects/FrameBuffer.cpp
        gfx/OpenGL/RenderBuffer.cpp
)

target_include_directories(Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Core
        PUBLIC
        Shared
        glm::glm
        PRIVATE
        glfw
        stb
)

target_compile_definitions(Core PRIVATE
        GLFW_INCLUDE_NONE
)
