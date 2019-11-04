//
// Created by Alexander Lyon on 2019-10-14.
//

#pragma once

#include <GLFW/glfw3.h>

GLFWwindow *initializeOpenGL();

void initializeUI(GLFWwindow *window);

GLuint initializeShaders(const char *vertexShaderPath, const char *fragmentShaderPath);

void teardown();