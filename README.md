# sog — **s**mall **o**penGL **g**raphics

## Write OpenGL. Don't speak OpenGL.

___

A small C++20 static library for OpenGL 4.6 Core Profile rendering: windowing, input,
cameras, type-safe GL object wrappers (buffers, shaders, textures, framebuffers, vertex arrays)
and uniform helpers. Direct State Access style throughout (GL 4.5+ entry points), with RAII
wrappers and exceptions (`sog::graphics_error`) instead of raw GL error checks.

Windows is the primary platform.

## Layout

```
opengl/
├── CMakeLists.txt            # project setup: C++20, output layout, deps, modules
├── dependencies.cmake        # setup_dependency() + stb, glm, glfw
├── Doxyfile                  # Doxygen configuration (output: docs/)
├── README.md
├── docs/                     # generated documentation (html/ + doxygen-warnings.log)
└── src/
    ├── CMakeLists.txt        # static library target "SOG"
    ├── sog/
    │   ├── include/sog/      # public headers (umbrella header: sog/sog.hpp, Error/, Utility/, gfx/)
    │   ├── gfx/              # windowing, camera, OpenGL object implementations
    │   ├── Utility/          # time, enum helpers (to_underlying), uniform upload helpers
    │   └── vendor/           # vendored glad + KHR (excluded from the docs)
    ├── examples/             # demo application (static lib + "Main" executable)
    └── tests/                # GoogleTest suite ("SOGTests" executable; excluded from the docs)
```

## Targets

| Target   | Kind                  | Links                        |
|----------|-----------------------|------------------------------|
| SOG      | static (the library)  | glm::glm, glfw (public), stb |
| Examples | static (demo objects) | SOG                          |
| Main     | executable (demo)     | Examples (+ copies assets/)  |
| SOGTests | executable (tests)    | SOG, GTest::gtest            |

The public include root is `src/sog/include`, so consumers write `#include <sog/sog.hpp>`
and link against `SOG`.

## Using the library

```cpp
#include <sog/sog.hpp>

int main()
{
    sog::Window window; // OpenGL 4.6 core, 800x600

    while (!window.shouldClose())
    {
        window.beginFrame(sog::ClearBuffer::Color | sog::ClearBuffer::Depth, {0, 0, 0, 255});
        // ... issue draw calls, e.g. sog::Window::draw(state)
        window.endFrame();
    }
}
```

See `src/examples/` for a complete scene: a textured cube rendered into an offscreen
framebuffer with a post-processing pass.

## Dependencies

All resolved by `setup_dependency()` in `dependencies.cmake`: a system-installed package is
tried first (`find_package`), otherwise the sources are downloaded with `FetchContent` at
configure time.

| Library                                            | Version | Purpose                        |
|----------------------------------------------------|---------|--------------------------------|
| [stb](https://github.com/nothings/stb)             | master  | image loading (stb_image)      |
| [glm](https://github.com/g-truc/glm)               | master  | math                           |
| [glfw](https://github.com/glfw/glfw)               | 3.5.1   | window, input, GL context      |
| [googletest](https://github.com/google/googletest) | 1.15.2  | unit tests (`SOG_BUILD_TESTS`) |

[glad](https://gen.glad.sh/) (OpenGL 4.6, Core Profile) is vendored under
`src/sog/vendor/` — no generation step needed.

## Requirements

- CMake **3.20+**
- C++20 compiler: MSVC 19.29+ (VS 2019 16.11+ / VS 2022), GCC 11+, Clang 16+
- Git (dependencies are cloned during configure)
- GPU/driver with OpenGL 4.6 support

## Building

```sh
cmake -S . -B build
cmake --build build --config Debug
```

`--config` only matters for multi-config generators (Visual Studio, Xcode).

Static libraries land in `build/lib`, executables in `build/bin` (`build/bin/<Config>` with
Visual Studio or Xcode). The first configure downloads glfw, glm and stb into `build/_deps/`.

## Testing

The `SOGTests` target (built by default; disable with `-DSOG_BUILD_TESTS=OFF`) is a GoogleTest
suite covering the whole library — the gfx/ wrappers, the Camera math, the exception types and
the draw helpers:

```sh
ctest --test-dir build --output-on-failure   # one process per test
build/bin/SOGTests                           # or everything in a single process
build/bin/SOGTests --gtest_filter=Window.*   # a single suite
```

The suite's custom `main` initializes GLFW once and creates every window/context with
`GLFW_VISIBLE=FALSE`, so it runs headless (CI-friendly). OpenGL results are verified for real:
draws are rasterized into an offscreen framebuffer and read back with `glReadPixels`, and GL
error state is asserted around every wrapped call.

## Documentation

The API documentation is generated with [Doxygen](https://www.doxygen.nl/):

```sh
doxygen Doxyfile
```

Output goes to `docs/html/index.html`; undocumented or malformed comments are reported in
`docs/doxygen-warnings.log` (kept empty).

## IDE

`CMAKE_EXPORT_COMPILE_COMMANDS` is enabled, producing `build/compile_commands.json` for
clangd-based tooling. CLion: open the project root and it will pick up `CMakeLists.txt`
automatically.

# The motivation

I've seen countless games being developed using libraries like SFML, Raylib, or perhaps just raw GLFW + GLAD. "They're
too simple," I thought. "They lack flexibility." Those were the kinds of things I kept telling myself whenever I started
a new project.

But that wasn't really the problem. I wanted *OpenGL*. I wanted to work directly with the *GPU*. OpenGL + GLAD gives me
exactly
that. However, who actually enjoys writing those unpleasant GLAD function calls? Nobody!

Of course, there are alternatives. Libraries like `bgfx` and `globjects` are amazing projects in their own right, but
they never quite felt right for what I wanted. I didn't want an overly complex API, nor did I want to sacrifice the
flexibility that comes with working directly with OpenGL. I simply wanted to use modern C++ alongside the latest OpenGL
features, without having to constantly speak the *OpenGL language*.

I wanted to create my own types of GPU buffers, work with OpenGL handles safely, manage resources without worrying about
their lifetime, and still have the freedom to write raw OpenGL whenever I felt like it.

*That* is the problem I wanted to solve.

# Has AI been used?

Yes, absolutely. And I want to be transparent about that.

I've used AI primarily for:

- **Code snippets:** Especially when dealing with functionality or concepts I don't fully understand yet.
- **Documentation:** Quite heavily, actually.
- **Tests:** Pretty much all of them. I could never be bothered to write tests myself!
- **CMake:** Certain configurations, functionality, and troubleshooting.
- **Boilerplate:** Saving time on repetitive code.
- **Debugging:** Identifying bugs and figuring out potential solutions.

I do want to put this into perspective, though. I'm *15 years old* as of writing this. I'm not a professional
programmer, nor do I have a team to work with. I understand that some people aren't particularly fond of AI-generated
code, and honestly, neither am I a fan of blindly relying on it.

However, I follow a certain philosophy: I want to use AI as a tool, not as a replacement for my own understanding. I
don't want to blindly copy code that I couldn't explain myself. I want to understand what goes into my projects and why
it works.

I try to write code that is readable, understandable, and doesn't require someone to spend more than five minutes
figuring out what's going on. I prioritize code flow, architecture, and readability over simply getting things to work.

At the end of the day, I want my projects to be something I'm proud of, and something that other people can actually
understand and work with.