add_library(App STATIC
        Application.cpp
)
set_target_properties(App PROPERTIES OUTPUT_NAME "sog")

target_link_libraries(App
        PRIVATE
        Core
        Shared
)
target_include_directories(App PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(App PUBLIC GLFW_INCLUDE_NONE)


add_executable(Main
        main.cpp
)

target_link_libraries(Main PRIVATE App)
add_custom_command(TARGET Main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:Main>/assets
        COMMENT Copying assets directory
)

