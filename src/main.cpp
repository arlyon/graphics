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
    renderable instancedFishModel = renderable("models/fish.obj", partyFish);

    // set up buffers for instancing
    GLuint modelBuffer;
    glGenBuffers(1, &modelBuffer);
    instancedFishModel.addVertexAttributeMatrix(3, modelBuffer);
    GLuint timeBuffer;
    glGenBuffers(1, &timeBuffer);
    instancedFishModel.addVertexAttributeFloat(7, timeBuffer);
    GLuint hueBuffer;
    glGenBuffers(1, &hueBuffer);
    instancedFishModel.addVertexAttributeFloat(8, hueBuffer);

    shader speaker = shader("shaders/vertex_speaker.glsl", "shaders/fragment_speaker.glsl");
    renderable cubeModel = renderable("models/cube.obj", speaker);

    auto cam = registry.create();
    registry.assign<position>(cam, glm::vec3(0,10,40), glm::quatLookAt(glm::normalize(glm::vec3(0,0.2,-0.8)), glm::vec3(0,1,0)));
    registry.assign<velocity>(cam, glm::vec3(0,0,0));
    registry.assign<camera>(cam, &settings.fov, window);

    for (int i : {0, 1, 2}) {
        auto speakerEntity = registry.create();
        registry.assign<position>(speakerEntity, glm::vec3(0,i * 1.5,0), glm::quatLookAt(glm::vec3(0,0,1), glm::vec3(0,1,0)));
        registry.assign<renderable>(speakerEntity, cubeModel);
    }

    double currentTime;
    double deltaTime;
    double lastTime = 0.0;

    while (!glfwWindowShouldClose(window)) {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        /* Run Systems */
        fish_physics(registry, deltaTime);
        physics(registry, deltaTime);
        fish_population(registry);
		boids(registry, &cam, deltaTime);

        auto color = settings.color;
        glClearColor(color[0], color[1], color[2], 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderRenderables(registry, &cam, deltaTime);
        renderFish(registry, &cam, deltaTime, partyFish, instancedFishModel, modelBuffer, timeBuffer, hueBuffer);
        if (settings.enable_menu) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            renderUI();
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            entity_control(registry, &cam, window, deltaTime);
        }

        GLenum error;
        while ((error = glGetError()) != GL_NO_ERROR) {
            std::cout << "Render Error: 0x" << std::hex << error << std::endl;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    teardown();
    instancedFishModel.close();
    cubeModel.close();
}

