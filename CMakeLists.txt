cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
project(aquarium
        VERSION "1.3.0"
        DESCRIPTION "Aquarium Demoscene For 3d Graphics"
        HOMEPAGE_URL "https://github.com/arlyon/graphics"
        )

if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
else ()
    message(WARNING "\
The file conanbuildinfo.cmake doesn't exist. \
Unless you are using a conan plugin, please run 'conan install' first. \
Refer to the readme for instructions.\
")
endif ()

add_executable(aquarium
        src/main.cpp
        src/initialize.cpp src/initialize.hpp
        src/settings.cpp src/settings.hpp
        src/components/components.cpp src/components/components.hpp
        src/components/render.cpp src/components/render.hpp
        src/components/physics.hpp
        src/systems/boids.cpp src/systems/boids.hpp
        src/systems/entity_control.cpp src/systems/entity_control.hpp
        src/systems/fish_population.cpp src/systems/fish_population.hpp
        src/systems/physics.cpp src/systems/physics.hpp
        src/systems/render.cpp src/systems/render.hpp

        lib/tiny_obj_loader.cpp lib/tiny_obj_loader.h
        lib/imgui_impl_glfw.cpp lib/imgui_impl_glfw.h
        lib/imgui_impl_opengl3.cpp lib/imgui_impl_opengl3.h)

target_compile_definitions(aquarium PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
target_link_libraries(aquarium CONAN_PKG::glfw CONAN_PKG::glad CONAN_PKG::glm CONAN_PKG::imgui CONAN_PKG::entt CONAN_PKG::stb)

# set compile options
if (MSVC)
    target_compile_options(aquarium PRIVATE /W4 /experimental:external /external:I $ENV{USERPROFILE}\\.conan /external:W0 /WX)
    set_target_properties(aquarium PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")
else ()
    target_compile_options(aquarium PRIVATE -Wall -Wextra -pedantic)
endif ()

# copy shaders and models on build
add_custom_target(copy_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/shaders" "${CMAKE_BINARY_DIR}/bin/shaders"
        COMMENT "Copy Shaders" VERBATIM)
add_custom_target(copy_models ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/models" "${CMAKE_BINARY_DIR}/bin/models"
        COMMENT "Copy Models" VERBATIM)
add_dependencies(aquarium copy_shaders)
add_dependencies(aquarium copy_models)

# install target
install(TARGETS aquarium DESTINATION .)
file(GLOB shaders "shaders/*")
install(FILES ${shaders} DESTINATION shaders)
file(GLOB models "models/*")
install(FILES ${models} DESTINATION models)

# cpack config
set(CPACK_PACKAGE_NAME "Aquarium")
set(CPACK_PACKAGE_VENDOR "Alexander Lyon")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/readme.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/license.txt")
set(CPACK_OUTPUT_FILE_PREFIX packages)
set(CPACK_PACKAGE_EXECUTABLES aquarium "Aquarium")
set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon.png")

if (APPLE)
    set(CPACK_BUNDLE_NAME "aquarium")
    set(CPACK_BUNDLE_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/resources/macOS.plist")
    set(CPACK_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon.icns")
    set(CPACK_GENERATOR "Bundle")
elseif (WIN32)
    set(CPACK_WIX_UPGRADE_GUID "b1f848cd-0554-4974-8437-ba0e3bb712cc")
    set(CPACK_GENERATOR "WIX")
endif ()

include(CPack)
