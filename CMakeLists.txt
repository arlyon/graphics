cmake_minimum_required(VERSION 3.15)
project(cpp_glfw_test)

set(CMAKE_CXX_STANDARD 17)

if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else ()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif ()

add_executable(cpp_glfw_test
        src/main.cpp
        src/systems/render.cpp src/systems/render.h
        src/systems/camera_orbit.cpp src/systems/camera_orbit.h
        src/systems/physics.cpp src/systems/physics.h
        src/systems/fish_population.cpp src/systems/fish_population.h
        src/initialize.cpp src/initialize.h
        src/settings.cpp src/settings.h
        src/components/model.h

        lib/tiny_obj_loader.cpp lib/tiny_obj_loader.h
        lib/imgui_impl_glfw.cpp lib/imgui_impl_glfw.h
        lib/imgui_impl_opengl3.cpp lib/imgui_impl_opengl3.h)

target_compile_definitions(cpp_glfw_test PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)
target_link_libraries(cpp_glfw_test ${CONAN_LIBS})

add_custom_target(copy_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/shaders" "${CMAKE_BINARY_DIR}/bin/shaders"
        COMMENT "Copy Shaders" VERBATIM)

add_custom_target(copy_models ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/models" "${CMAKE_BINARY_DIR}/bin/models"
        COMMENT "Copy Models" VERBATIM)

ADD_DEPENDENCIES(cpp_glfw_test copy_shaders)
ADD_DEPENDENCIES(cpp_glfw_test copy_models)
