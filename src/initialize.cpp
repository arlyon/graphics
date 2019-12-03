//
// Created by Alexander Lyon on 2019-10-14.
//

#include <iostream>
#include <fstream>
#include <vector>

#include <imgui.h>
#include <GL/glew.h>
#include <entt/entt.hpp>

#include "../lib/imgui_impl_glfw.h"
#include "../lib/imgui_impl_opengl3.h"
#include "initialize.hpp"
#include "settings.hpp"
#include "systems/entity_control.hpp"

void GLAPIENTRY MessageCallback(GLenum,
                                GLenum type,
                                GLuint,
                                GLenum severity,
                                GLsizei,
                                const GLchar *message,
                                const void *) {
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}

GLFWwindow *initializeOpenGL() {
    if (glfwInit() != GLFW_TRUE) {
        std::cerr << "Couldn't initialize glfw" << std::endl;
        std::exit(-1);
    }

#if __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    if (glfwInit() != GLFW_TRUE) {
        std::cerr << "Couldn't initialize glfw" << std::endl;
        std::exit(-1);
    }

    GLFWwindow *window;
    window = glfwCreateWindow(640, 480, "Hello Sea++", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Couldn't create the glfw window" << std::endl;
        glfwTerminate();
        std::exit(-1);
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err) {
        std::cerr << "Couldn't initialize glew" << std::endl;
        std::cerr << glewGetErrorString(err);
        glfwTerminate();
        std::exit(-1);
    }

#ifndef NDEBUG
    // Output some debugging information
    std::cout << "VENDOR: " << (char *) glGetString(GL_VENDOR) << std::endl;
    std::cout << "VERSION: " << (char *) glGetString(GL_VERSION) << std::endl;
    std::cout << "RENDERER: " << (char *) glGetString(GL_RENDERER) << std::endl;

#if __APPLE__
#else
    glDebugMessageCallback(MessageCallback, 0);
    glEnable(GL_DEBUG_OUTPUT);
#endif
#endif

    // render fragments in the correct order
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    return window;
}

void initializeUI(GLFWwindow *window) {
    IMGUI_CHECKVERSION();

#if __APPLE__
    const char *glsl_version = "#version 410";
#else
    const char *glsl_version = "#version 430";
#endif

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void initializeInput(GLFWwindow *window) {
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
}

void teardown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}
