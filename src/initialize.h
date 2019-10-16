//
// Created by Alexander Lyon on 2019-10-14.
//

#pragma once

#include <GLFW/glfw3.h>
#include "../lib/ECS.h"

GLFWwindow *initializeOpenGL();

void initializeUI(GLFWwindow *window);

GLuint initializeShaders(const char *vertexShaderPath, const char *fragmentShaderPath);

ECS::World *initializeWorld();

void teardown(ECS::World *world, GLuint vertexbuffer, GLuint vertexArrayId, GLuint programID);