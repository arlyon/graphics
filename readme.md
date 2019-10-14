# Graphics Example

Requires a graphics card with OpenGL 4.1 or above.

- INSTANCING MORE MARKS

## Building

This project uses conan to manage the dependencies. All the 
packages are managed in `conanfile.txt`. To use it with cmake,
install the dependencies and then run as usual.

The project uses cmake, with the same folders as those clion uses.

### Release

```bash
conan install . -s build_type=Release --install-folder=cmake-build-release
cmake -DCMAKE_BUILD_TYPE=Release -B cmake-build-release
cmake --build cmake-build-release --target cpp_glfw_test
```

### Debug

```bash
conan install . -s build_type=Debug --install-folder=cmake-build-debug
cmake -DCMAKE_BUILD_TYPE=Debug -B cmake-build-debug
cmake --build cmake-build-debug --target cpp_glfw_test
```

### CLion

To set up with CLion, just run the conan install commands and reload
the cmake project.

```bash
conan install . -s build_type=Release --install-folder=cmake-build-release
conan install . -s build_type=Debug --install-folder=cmake-build-debug
```

## Libraries

| Library | Purpose |
| ------- | ------- |
| GLFW    | Lightweight cross-platform library for handling OpenGL windows, context, and input |
| GLEW    | Cross-platform library for determining available OpenGL extensions |
| GLM     | Cross-platform math library for use with OpenGL |
| imgui   | A simple UI library |