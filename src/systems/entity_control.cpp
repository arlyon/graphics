//
// Created by Alexander Lyon on 2019-10-11.
//

#include <iostream>
#include <queue>

#include <GL/glew.h>

#include "entity_control.hpp"
#include "../components/components.hpp"
#include "../settings.hpp"

struct move_event {
    double x;
    double y;
};

struct scroll_event {
    double x;
    double y;
};

struct mouse_state {
    std::queue<move_event> move;
    std::queue<scroll_event> scroll;
    double mouse_x;
    double mouse_y;
    bool first_mouse = true;
};

mouse_state state;

void mouse_callback(GLFWwindow *, double xpos, double ypos) {
    state.move.push({xpos, ypos});
}

void scroll_callback(GLFWwindow *, double xoffset, double yoffset) {
    state.scroll.push({xoffset, yoffset});
}

void key_callback(GLFWwindow *, int key, int, int action, int) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        auto &settings = Settings::getInstance();
        settings.enable_menu = !settings.enable_menu;
    }
}

/**
 * Controls the given entity.
 */
void entity_control(entt::registry &registry, entt::entity *cam, GLFWwindow *window, double deltaTime) {
    Settings &s = Settings::getInstance();
    auto &vel = registry.get<velocity>(*cam);
    auto &pos = registry.get<position>(*cam);

    // pitch/yaw
    while (!state.move.empty())
    {
        auto event = state.move.front();

        if (!state.first_mouse) {
            double x_offset = (event.x - state.mouse_x) * s.mouse_sensitivity;
            double y_offset = (state.mouse_y - event.y) * s.mouse_sensitivity;
            pos.orientation *= glm::quat(glm::vec3(-y_offset, x_offset, 0) * pos.orientation);
        } else {
            state.first_mouse = false;
        }

        state.mouse_x = event.x;
        state.mouse_y = event.y;
        state.move.pop();
    }

    // roll
    float roll = 0;
    if (glfwGetKey(window, GLFW_KEY_Q)) roll -= 1;
    if (glfwGetKey(window, GLFW_KEY_E)) roll += 1;
    pos.orientation *= glm::quat(glm::vec3(0, 0, roll * deltaTime) * pos.orientation);

    // strafe
    glm::vec3 strafe = {};
    if (glfwGetKey(window, GLFW_KEY_W)) strafe += glm::vec3(0, 0, -1);
    if (glfwGetKey(window, GLFW_KEY_A)) strafe += glm::vec3(-1, 0, 0);
    if (glfwGetKey(window, GLFW_KEY_S)) strafe += glm::vec3(0, 0, 1);
    if (glfwGetKey(window, GLFW_KEY_D)) strafe += glm::vec3(1, 0, 0);
    if (glfwGetKey(window, GLFW_KEY_SPACE)) strafe += glm::vec3(0, 1, 0);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) strafe += glm::vec3(0, -1, 0);
    vel.velocity += strafe * pos.orientation * (float)deltaTime * 2.0f; // move in the facing direction

    // fov
    while (!state.scroll.empty())
    {
        auto event = state.scroll.front();
        s.fov += (float)event.y * (float)deltaTime * 2.0f;
        if (s.fov < 1.0f) s.fov = 1.0f;
        if (s.fov > 120.0f) s.fov = 120.0f;
        state.scroll.pop();
    }
}