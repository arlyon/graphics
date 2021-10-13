<p align="center"><img src="./splash.jpg" /></p>

# Aquarium

![version](https://img.shields.io/github/v/release/arlyon/graphics)
![date](https://img.shields.io/badge/platform-macOS%20%7C%20linux%20%7C%20windows-lightgrey)
![build](https://img.shields.io/github/workflow/status/arlyon/graphics/CI)
![license](https://img.shields.io/github/license/arlyon/graphics)

Aquarium is a cross-platform light-weight ECS games engine that is used 
to build a simple demoscene. It is powered by OpenGL 4 and ENTT. The 
latest release is available for Windows, macOS, and Linux in the 
[releases](https://github.com/arlyon/graphics/releases) section.

## Setup

### Prerequisites

To run this software you must have:

- [a copy](https://github.com/arlyon/graphics/releases)
- vcredist140 (windows only)

To build this software you must have:

- a cpp compiler
- a recent cmake
- a recent python

### Tools

This project uses cmake, a meta build tool, to generate the correct 
build scripts for your platform. The library dependencies are handled 
via [`conan`](https://github.com/conan-io/conan), a package manager for 
c++.

```bash
pip install conan cmake
```

### Libraries

This project uses a few libraries. Most of the packages are listed in 
`conanfile.txt`, however `tiny_obj_loader` is not on conan.

| Library | Purpose | License |
| ------- | ------- | ------- |
| GLFW    | Lightweight cross-platform library for handling OpenGL windows, context, and input | zlib/libpng |
| glad    | Cross-platform library for determining available OpenGL extensions | MIT |
| GLM     | Cross-platform math library for use with OpenGL | MIT |
| imgui   | A simple UI library | MIT |
| entt    | Simple ECS library for data-oriented programming | MIT |
| stb     | Various single file libraries. Used for image loading. | Public Domain |
| tiny_obj_loader | A header-only library for loading obj meshes | MIT |

## Building

### Release

```bash
conan install . -s build_type=Release --install-folder=release --build=missing
cmake -B release
cmake --build release --target aquarium --config Release

# alternatively...
cmake --build release --target package --config Release
```

### Debug

```bash
conan install . -s build_type=Debug --install-folder=debug --build=missing
cmake -B debug
cmake --build debug --target aquarium --config Debug
```

## IDE Setup

### Visual Studio 2019

The easiest way to get set up with Visual Studio is to install the
conan extension which will download and manage all your dependencies
automatically. First, generate a solution and simply open it.

```bash
cmake . -G "Visual Studio 16 2019"
```

Then install the [conan extension](https://github.com/conan-io/conan-vs-extension)
and, in the solution explorer, right click 'aquarium' and select the 'Install'
opton.

> Note: If your conan cache is installed anywhere other than the default,
        you must change the MSVC target compile options in CMakeLists to 
        reflect it or else errors from the dependencies will be reported.

### CLion

To set up with CLion, just run conan, installing into the relevant
`cmake-build-x` folder. Then, reload the cmake project and you can build.

```bash
conan install . -s build_type=Release --install-folder=cmake-build-release
conan install . -s build_type=Debug --install-folder=cmake-build-debug
```
