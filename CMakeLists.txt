cmake_minimum_required(VERSION 3.20)

project(sog
        VERSION 0.1.0
        DESCRIPTION "Small OpenGL Graphics"
        LANGUAGES C CXX
)

# ---------------------------------------------------------------------------
# Language
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default to Debug for single-config generators when no build type was given.
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
            Debug Release RelWithDebInfo MinSizeRel
    )
endif ()

# ---------------------------------------------------------------------------
# Output layout (runtime = executable and DLLs, so they stay side by side)
# ---------------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------------------------
# Third-party dependencies (stb, glm, glfw, googletest)
# ---------------------------------------------------------------------------
option(SOG_BUILD_TESTS "Build the sog test suite (src/tests)" ON)

include("${CMAKE_CURRENT_SOURCE_DIR}/dependencies.cmake")

add_subdirectory(src)
add_subdirectory(src/examples)

if (SOG_BUILD_TESTS)
    enable_testing()
    add_subdirectory(src/tests)
endif ()
