//
// Created by Alexander Lyon on 2019-10-11.
//

#pragma once

#include <GLFW/glfw3.h>
#include <entt/entt.hpp>

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

void entity_control(entt::registry &registry, entt::entity *cam, GLFWwindow *window, GLfloat deltaTime);

