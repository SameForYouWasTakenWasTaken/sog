# sog — Small OpenGL Game

Cross-platform OpenGL **4.6 Core Profile** project skeleton built with CMake and C++20.
Targets Windows, Linux and macOS, with Windows as the primary platform.

## Layout

```
opengl/
├── CMakeLists.txt            # project setup: C++20, output layout, deps, modules
├── dependencies.cmake        # setup_dependency() + stb, glm, glfw
├── README.md
└── src/
    ├── CMakeLists.txt        # Shared → Core → App
    ├── Shared/               # base layer, links nothing
    │   ├── CMakeLists.txt    # static library "Shared"
    │   ├── Types.hpp / Types.cpp
    │   └── vendor/           # generated glad header(s) go here
    ├── Core/                 # engine layer
    │   ├── CMakeLists.txt    # static library "Core"
    │   └── Engine.hpp / Engine.cpp
    └── App/                  # executable
        ├── CMakeLists.txt    # executable target "App", output name "sog"
        └── Application.hpp / Application.cpp
```

## Modules

| Target | Kind     | Links                        |
|--------|----------|------------------------------|
| Shared | static   | —                            |
| Core   | static   | Shared, glm::glm, stb        |
| App    | executable (`sog`) | Core, Shared, glfw |

## Dependencies

All resolved by `setup_dependency()` in `dependencies.cmake`: a system-installed
package is tried first (`find_package`), otherwise the sources are downloaded
with `FetchContent` at configure time.

| Library                              | Version | Purpose                     |
|--------------------------------------|---------|-----------------------------|
| [stb](https://github.com/nothings/stb) | master  | single-header utilities (image loading, …) |
| [glm](https://github.com/g-truc/glm)   | master  | math                        |
| [glfw](https://github.com/glfw/glfw)   | 3.4     | window, input, GL context   |

## Requirements

- CMake **3.20+**
- C++20 compiler: MSVC 19.29+ (VS 2019/2022), GCC 11+, Clang 14+, AppleClang 13+
- Git (dependencies are cloned during configure)
- GPU/driver with OpenGL 4.6 support

## Building

```sh
cmake -S . -B build
cmake --build build --config Debug
```

`--config` only matters for multi-config generators (Visual Studio, Xcode).

The executable lands in `build/bin` (`build/bin/<Config>` with Visual Studio or
Xcode). The first configure downloads glfw, glm and stb into `build/_deps/`.

### glad

Generate a loader at [gen.glad.sh](https://gen.glad.sh/) (OpenGL 4.6, Core
Profile) and drop the generated files into `src/Shared/vendor/` — that
directory is on Shared's public include path, so every module can include them.

## IDE

`CMAKE_EXPORT_COMPILE_COMMANDS` is enabled, producing
`build/compile_commands.json` for clangd-based tooling. CLion: open the project
root and it will pick up `CMakeLists.txt` automatically.
