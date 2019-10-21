# Graphics Example

Requires a graphics card with OpenGL 4.1 or above.

- INSTANCING MORE MARKS

## Requirements

To build this software you must have

- a recent cmake
- a recent python
- the software dependencies

### Tools

This project uses cmake, the meta build tool, to generate
the correct build scripts for your platform. You can get a copy [here](https://cmake.org).

Additionally, software dependencies are handled via [`conan`](https://github.com/conan-io/conan),
a package manager for c++. You can install this with pip.

```bash
pip install conan
```

### Libraries

This project uses a few libraries. This is how you properly compile them.
All the packages are managed in `conanfile.txt`.

| Library | Purpose | License |
| ------- | ------- | ------- |
| GLFW    | Lightweight cross-platform library for handling OpenGL windows, context, and input | zlib/libpng |
| GLEW    | Cross-platform library for determining available OpenGL extensions | MIT |
| GLM     | Cross-platform math library for use with OpenGL | MIT |
| imgui   | A simple UI library | MIT |
| entt    | Simple ECS library for data-oriented programming | MIT |
| obj_loader | A header-only library for loading obj meshes | MIT |


## Building

### Release

```bash
conan install . -s build_type=Release --install-folder=release --build=missing
cmake -DCMAKE_BUILD_TYPE=Release -B release
cmake --build release --target cpp_glfw_test
```

### Debug

```bash
conan install . -s build_type=Debug --install-folder=debug --build=missing
cmake -DCMAKE_BUILD_TYPE=Debug -B debug
cmake --build debug --target cpp_glfw_test
```

### Visual Studio 2019

The easiest way to get set up with Visual Studio is to install the
conan extension which will download and manage all your dependencies
automatically. First, generate a solution and simply open it.

```bash
cmake -G "Visual Studio 16 2019"
```

### CLion

To set up with CLion, just run conan, installing into the relevant
`cmake-build-x` folder. Then, reload the cmake project and you can build.

```bash
conan install . -s build_type=Release --install-folder=cmake-build-release
conan install . -s build_type=Debug --install-folder=cmake-build-debug
```
