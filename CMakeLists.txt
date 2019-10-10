cmake_minimum_required(VERSION 3.15)
project(cpp_glfw_test)

set(CMAKE_CXX_STANDARD 14)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif()

add_executable(cpp_glfw_test
        main.cpp
        bindings/imgui_impl_glfw.cpp
        bindings/imgui_impl_glfw.h
        bindings/imgui_impl_opengl3.cpp
        bindings/imgui_impl_opengl3.h)

target_compile_definitions(cpp_glfw_test PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)
target_link_libraries(cpp_glfw_test ${CONAN_LIBS})