/**
 * Right handed system,
 * glm::vec3(x, y, z) where:
 *   - x+ is right
 *   - y+ is up
 *   - z+ is out of the screen
 */

#include <iostream>
#include <variant>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <imgui.h>

#include "initialize.hpp"
#include "settings.hpp"
#include "components/components.hpp"
#include "systems/render.hpp"
#include "systems/entity_control.hpp"
#include "systems/fish_population.hpp"
#include "systems/physics.hpp"
#include "systems/boids.hpp"

int main() {
    auto &settings = Settings::getInstance();
    auto registry = entt::registry{};

    auto *window = initializeOpenGL();
    initializeUI(window);
    initializeInput(window);

    shader partyFish = shader("shaders/vertex_fish.glsl", "shaders/fragment_party_fish.glsl");
    renderable fishModel = renderable("models/fish.obj", partyFish);
    shader speaker = shader("shaders/vertex_fish.glsl", "shaders/fragment_speaker.glsl");
    renderable cubeModel = renderable("models/cube.obj", speaker);

    auto cam = registry.create();
    registry.assign<position>(cam, glm::vec3(0,10,40), glm::quatLookAt(glm::vec3(0,0.2,-0.8), glm::vec3(0,1,0)));
    registry.assign<velocity>(cam, glm::vec3(0,0,0));
    registry.assign<camera>(cam, &settings.fov, window);

    auto cube = registry.create();
    registry.assign<position>(cube, glm::vec3(0,0,0), glm::quatLookAt(glm::vec3(0,0,-1), glm::vec3(0,1,0)));
    registry.assign<renderable>(cube, cubeModel);

    auto origin = registry.create();
    registry.assign<position>(origin, glm::vec3(0,1.5,0), glm::quatLookAt(glm::vec3(0,0,-1), glm::vec3(0,1,0)));
    registry.assign<renderable>(origin, cubeModel);

    auto origin1 = registry.create();
    registry.assign<position>(origin1, glm::vec3(0,-1.5,0), glm::quatLookAt(glm::vec3(0,0,-1), glm::vec3(0,1,0)));
    registry.assign<renderable>(origin1, cubeModel);

    double currentTime;
    double deltaTime;
    double lastTime = 0.0;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        /* Run Systems */
        fish_physics(registry, deltaTime);
        physics(registry, deltaTime);
        fish_population(registry, fishModel);
		boids(registry, &cam, deltaTime);

        render(registry, &cam, deltaTime);
        if (settings.enable_menu) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            renderUI();
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            entity_control(registry, &cam, window, deltaTime);
        }

        glfwSwapBuffers(window);
    }

    teardown();
    fishModel.close();
}

