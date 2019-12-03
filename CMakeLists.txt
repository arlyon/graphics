cmake_minimum_required(VERSION 3.15)
project(aquarium)

set(CMAKE_CXX_STANDARD 17)

if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else ()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif ()

add_executable(aquarium
        src/main.cpp
        src/initialize.cpp src/initialize.hpp
        src/settings.cpp src/settings.hpp
        src/components/components.cpp src/components/components.hpp
		src/systems/boids.cpp src/systems/boids.hpp
        src/systems/entity_control.cpp src/systems/entity_control.hpp
        src/systems/fish_population.cpp src/systems/fish_population.hpp
        src/systems/physics.cpp src/systems/physics.hpp
        src/systems/render.cpp src/systems/render.hpp
		src/util/octree.cpp src/util/octree.hpp

        lib/tiny_obj_loader.cpp lib/tiny_obj_loader.h
        lib/imgui_impl_glfw.cpp lib/imgui_impl_glfw.h
        lib/imgui_impl_opengl3.cpp lib/imgui_impl_opengl3.h)

target_compile_definitions(aquarium PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)
target_link_libraries(aquarium ${CONAN_LIBS})

if ( MSVC )
    target_compile_options(aquarium PRIVATE /W4 /WX)
else()
    target_compile_options(aquarium PRIVATE -Wall -Wextra -pedantic)
endif()

add_custom_target(copy_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/shaders" "${CMAKE_BINARY_DIR}/bin/shaders"
        COMMENT "Copy Shaders" VERBATIM)

add_custom_target(copy_models ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/models" "${CMAKE_BINARY_DIR}/bin/models"
        COMMENT "Copy Models" VERBATIM)

ADD_DEPENDENCIES(aquarium copy_shaders)
ADD_DEPENDENCIES(aquarium copy_models)
