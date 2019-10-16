//
// Created by Alexander Lyon on 2019-10-11.
//

#pragma once

#include <GLFW/glfw3.h>
#include "../lib/ECS.h"

void render(ECS::World *world, GLFWwindow *window, GLuint vertexBuffer, GLuint shaderProgramID);
void renderUI();

