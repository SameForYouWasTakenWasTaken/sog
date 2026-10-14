add_library(Examples STATIC
        Application.cpp
)
# Don't overwrite the core library's name! Let it output libExamples.a or name the target appropriately:
set_target_properties(Examples PROPERTIES OUTPUT_NAME "SOG-Examples")

target_link_libraries(Examples
        PRIVATE
        SOG
)
target_include_directories(Examples PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Main
        main.cpp
)

target_link_libraries(Main PRIVATE Examples)
add_custom_command(TARGET Main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:Main>/assets
        COMMENT Copying assets directory
)