#include <iostream>
#include <variant>

#include <GL/glew.h>
#include <imgui.h>

#include "systems/render.hpp"
#include "systems/camera_orbit.hpp"
#include "settings.hpp"
#include "components/components.hpp"
#include "systems/camera_orbit.hpp"
#include "systems/render.hpp"
#include "systems/fish_population.hpp"
#include "systems/physics.hpp"
#include "initialize.hpp"

int main() {
    auto &settings = Settings::getInstance();
    auto registry = entt::registry{};

    auto *window = initializeOpenGL();
    initializeUI(window);

    renderable fishModel = renderable("models/fish.obj", "shaders/vertex_fish.glsl", "shaders/fragment_fish.glsl");

    auto cam = registry.create();
    registry.assign<position>(cam, glm::vec3(2, 0, 5));
    registry.assign<camera>(cam, &settings.fov, window);

    GLfloat currentTime;
    GLfloat deltaTime;
    GLfloat lastTime = 0.0;

    while (!glfwWindowShouldClose(window)) {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        /* Run Systems */
        camera_orbit(registry, deltaTime);
        physics(registry, deltaTime);

        fish_population(registry, fishModel);
        render(registry, &cam, currentTime);
        if (settings.enable_menu) renderUI();

        glfwSwapBuffers(window);
    }

    teardown();
    fishModel.close();
}

