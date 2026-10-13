# Shared: base layer. Links nothing.
add_library(Shared STATIC
        GlobalTypes.hpp
        vendor/glad/gl.h
        vendor/glad/gl.c
        vendor/KHR/khrplatform.h
        Types/Math.hpp
)

target_link_libraries(Shared PRIVATE
        glm::glm
)

# vendor/ holds the generated glad header(s); PUBLIC so Core and App can
# include them as well.
target_include_directories(Shared PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor"
)
