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

void GLAPIENTRY MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar *message,
                const void *userParam) {
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}

GLFWwindow *initializeOpenGL() {
    glewExperimental = GL_TRUE;
    if (glfwInit() != GLFW_TRUE) {
        std::cerr << "Couldn't initialize glfw" << std::endl;
        std::exit(-1);
    }

#if __APPLE__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
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
    // GL 3.2 + GLSL 150
    const char *glsl_version = "#version 410";
#else
    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
#endif

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS) {
            auto *settings = static_cast<Settings *>(glfwGetWindowUserPointer(window));
            settings->enable_menu = !settings->enable_menu;
        }
    });
}

void teardown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}
