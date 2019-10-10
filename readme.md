# Graphics Example

Requires a graphics card with OpenGL 4.1 or above.

## Install

This project uses conan to manage the dependencies. All the 
packages are managed in `conanfile.txt`. To use it with cmake,
install the dependencies and then run as usual.

```bash
conan install . -s build_type=Debug --install-folder=cmake-build-debug
conan install . -s build_type=Release --install-folder=cmake-build-release

cmake --build cmake-build-debug --target cpp_glfw_test
```

- INSTANCING MORE MARKS

## Libraries

| Library | Purpose |
| ------- | ------- |
| GLFW    | Lightweight cross-platform library for handling OpenGL windows, context, and input |
| GLEW    | Cross-platform library for determining available OpenGL extensions |
| GLM     | Cross-platform math library for use with OpenGL |
| imgui   | A simple UI library |