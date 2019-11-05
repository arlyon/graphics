#include <iostream>
#include <variant>

#include <GL/glew.h>
#include <imgui.h>

#include "systems/render.hpp"
#include "systems/camera_orbit.hpp"
#include "settings.hpp"
#include "components/components.hpp"
#include "../lib/tiny_obj_loader.h"
#include "systems/fish_population.hpp"
#include "systems/physics.hpp"
#include "initialize.hpp"

int main() {
    auto &settings = Settings::getInstance();
    auto registry = entt::registry{};

    auto *window = initializeOpenGL();
    glfwSetWindowUserPointer(window, &settings); // add settings to window
    initializeUI(window);

	renderable fishModel = renderable("models/fish.obj", "shaders/vertex_simple.glsl", "shaders/fragment_simple.glsl");

	auto cam = registry.create();
	registry.assign<position>(cam, glm::vec3(4,3,3));
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
        render(registry, &cam);
        if (settings.enable_menu) renderUI();

        glfwSwapBuffers(window);
    }

    teardown();
	fishModel.close();
}

