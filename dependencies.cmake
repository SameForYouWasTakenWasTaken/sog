# =============================================================================
# Third-party dependencies for sog.
#
# Every external library is resolved through setup_dependency().
# =============================================================================

include(FetchContent)

#[==[
setup_dependency
================

Resolves one third-party dependency. A package already installed on the system
is preferred; otherwise the sources are downloaded at configure time with
FetchContent and joined into this build.

Usage
-----

    setup_dependency(
        NAME <name>                 # Unique dependency id; also the default target name.
        [FIND_PACKAGE <package>]    # Tried first via find_package(<package> CONFIG QUIET).
        [TARGET <target>]           # Target that must exist afterwards (default: NAME).
        [GIT_REPOSITORY <url>]      # Used when the package is not installed.
        [GIT_TAG <ref>]             # Branch, tag or commit to check out.
        [SOURCE_SUBDIR <dir>]       # Subdirectory holding the dependency's CMakeLists.txt.
        [HEADER_ONLY]               # Checkout ships no build system; expose it as an INTERFACE target.
        [INCLUDE_SUBDIR <dir>]      # Include root relative to the checkout (HEADER_ONLY only).
        [OPTIONS <var> <value> ...] # <variable> <value> pairs cached before the dependency builds.
    )

Behavior
--------

1. If <TARGET> already exists, the call returns immediately.
2. If FIND_PACKAGE is given, a quiet CONFIG lookup runs; if it provides
   <TARGET>, the call returns.
3. Otherwise the sources are fetched with FetchContent. The dependency's own
   CMakeLists.txt is added when one exists (never the case for HEADER_ONLY).
4. With HEADER_ONLY, the checkout is wrapped in an INTERFACE library named
   <TARGET> that exports the checkout (or INCLUDE_SUBDIR) as its include path.
5. If <TARGET> still does not exist, configuration fails with an error that
   points back at this call.

The OPTIONS values are written into the cache before the dependency's own
CMakeLists.txt runs, so they reliably override its option() defaults.
Boolean-looking values (ON/OFF/TRUE/FALSE/YES/NO/1/0) get BOOL type,
everything else gets STRING type.

Example
-------

    setup_dependency(
        NAME glfw
        FIND_PACKAGE glfw3
        TARGET glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
        OPTIONS
            GLFW_BUILD_EXAMPLES OFF
            GLFW_BUILD_TESTS OFF
    )
]==]
function(setup_dependency)
    cmake_parse_arguments(PARSE_ARGV 0 DEP
            "HEADER_ONLY"
            "NAME;FIND_PACKAGE;TARGET;GIT_REPOSITORY;GIT_TAG;SOURCE_SUBDIR;INCLUDE_SUBDIR"
            "OPTIONS"
    )

    if (NOT DEP_NAME)
        message(FATAL_ERROR "setup_dependency(): NAME is required")
    endif ()

    if (NOT DEP_TARGET)
        set(DEP_TARGET "${DEP_NAME}")
    endif ()

    # 1) Provided by an earlier call or by the parent project?
    if (TARGET "${DEP_TARGET}")
        return()
    endif ()

    # 2) Installed on this machine?
    if (DEP_FIND_PACKAGE)
        find_package("${DEP_FIND_PACKAGE}" CONFIG QUIET)
        if (TARGET "${DEP_TARGET}")
            message(STATUS "setup_dependency(${DEP_NAME}): using installed '${DEP_FIND_PACKAGE}'")
            return()
        endif ()
    endif ()

    # 3) Not installed: fetch the sources.
    if (NOT DEP_GIT_REPOSITORY)
        message(FATAL_ERROR
                "setup_dependency(${DEP_NAME}): target '${DEP_TARGET}' not found on this system "
                "and no GIT_REPOSITORY was given"
        )
    endif ()

    if (DEP_OPTIONS)
        list(LENGTH DEP_OPTIONS _optionCount)
        math(EXPR _isOdd "${_optionCount} % 2")
        if (_isOdd)
            message(FATAL_ERROR
                    "setup_dependency(${DEP_NAME}): OPTIONS expects <variable> <value> pairs"
            )
        endif ()

        set(_remaining ${DEP_OPTIONS})
        while (_remaining)
            list(POP_FRONT _remaining _variable _value)
            if (_value MATCHES "^(ON|OFF|TRUE|FALSE|YES|NO|1|0)$")
                set(${_variable} "${_value}" CACHE BOOL
                        "Required by setup_dependency(${DEP_NAME})" FORCE)
            else ()
                set(${_variable} "${_value}" CACHE STRING
                        "Required by setup_dependency(${DEP_NAME})" FORCE)
            endif ()
        endwhile ()
    endif ()

    set(_declaration "")
    if (DEP_GIT_TAG)
        list(APPEND _declaration GIT_TAG "${DEP_GIT_TAG}")
    endif ()
    if (DEP_SOURCE_SUBDIR)
        list(APPEND _declaration SOURCE_SUBDIR "${DEP_SOURCE_SUBDIR}")
    endif ()

    # CMake >= 4.0 refuses dependencies that declare a policy version older than
    # 3.5 (GLFW 3.4 declares 3.4). Raising the floor keeps such checkouts
    # configurable without patching them; it never lowers or otherwise changes
    # a version the dependency already declares higher.
    if (CMAKE_VERSION VERSION_GREATER_EQUAL "4.0" AND NOT DEFINED CMAKE_POLICY_VERSION_MINIMUM)
        set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
    endif ()

    FetchContent_Declare(${DEP_NAME}
            GIT_REPOSITORY "${DEP_GIT_REPOSITORY}"
            ${_declaration}
    )
    FetchContent_MakeAvailable(${DEP_NAME})

    # 4) Header-only checkouts have no build system to add; wrap them ourselves.
    if (DEP_HEADER_ONLY)
        if (NOT TARGET "${DEP_TARGET}")
            set(_includeRoot "${${DEP_NAME}_SOURCE_DIR}")
            if (DEP_INCLUDE_SUBDIR)
                string(APPEND _includeRoot "/${DEP_INCLUDE_SUBDIR}")
            endif ()
            add_library("${DEP_TARGET}" INTERFACE)
            target_include_directories("${DEP_TARGET}" INTERFACE "${_includeRoot}")
            message(STATUS "setup_dependency(${DEP_NAME}): header-only target '${_includeRoot}'")
        endif ()
    endif ()

    # 5) The dependency must provide its target by now.
    if (NOT TARGET "${DEP_TARGET}")
        message(FATAL_ERROR
                "setup_dependency(${DEP_NAME}): expected target '${DEP_TARGET}' does not exist. "
                "Check FIND_PACKAGE, TARGET and GIT_REPOSITORY arguments."
        )
    endif ()
endfunction()

# -----------------------------------------------------------------------------
# Dependencies of sog
# -----------------------------------------------------------------------------

# stb - single-header utilities (stb_image.h and friends).
setup_dependency(
        NAME stb
        TARGET stb
        HEADER_ONLY
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG master
)

# glm - math library, consumed through the glm::glm target.
setup_dependency(
        NAME glm
        FIND_PACKAGE glm
        TARGET glm::glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG master
        OPTIONS
        GLM_BUILD_TESTS OFF
)

# glfw - window creation, input and OpenGL context.
setup_dependency(
        NAME glfw
        FIND_PACKAGE glfw3
        TARGET glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.5.1
        OPTIONS
        GLFW_BUILD_EXAMPLES OFF
        GLFW_BUILD_TESTS OFF
        GLFW_BUILD_DOCS OFF
        GLFW_INSTALL OFF
)

# googletest - unit test framework for the SOG test suite (src/tests).
# Only resolved when the test suite is enabled (SOG_BUILD_TESTS).
# Both resolution paths provide GTest::gtest: an installed GTest exports it from
# its config package, and googletest's own CMake creates it as an ALIAS of gtest.
if (SOG_BUILD_TESTS)
    setup_dependency(
            NAME googletest
            FIND_PACKAGE GTest
            TARGET GTest::gtest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.15.2
            OPTIONS
            INSTALL_GTEST OFF
            BUILD_GMOCK OFF
            gtest_force_shared_crt ON
    )
endif ()
