//
// Created by Alexander Lyon on 2019-10-14.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <imgui.h>
#include <GL/glew.h>
#include <entt/entt.hpp>

#include "../lib/imgui_impl_glfw.h"
#include "../lib/imgui_impl_opengl3.h"
#include "initialize.h"
#include "settings.h"

GLuint linkProgram(GLuint vertexShaderID, GLuint fragmentShaderID);
GLuint compileShader(const char *shaderPath, GLuint shaderType);

GLFWwindow *initializeOpenGL() {
    GLFWwindow *window;

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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Couldn't create the glfw window" << std::endl;
        glfwTerminate();
        std::exit(-1);
    }

    glfwMakeContextCurrent(window);

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
#endif

    // glClearColor(0.8f, 0.3f, 0.1f, 0.0f);

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

    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS) {
            auto *settings = static_cast<Settings*>(glfwGetWindowUserPointer(window));
            settings->enable_menu = !settings->enable_menu;
        }
    });
}

/**
 * Initialize the shaders. Adapted from
 * https://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
 * to load all the shaders in the shaders directory.
 *
 * todo: support globbing
 */
GLuint initializeShaders(const char *vertexShaderPath, const char *fragmentShaderPath) {
    GLuint vertexShaderID = compileShader(vertexShaderPath, GL_VERTEX_SHADER);
    GLuint fragmentShaderID = compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
    return linkProgram(vertexShaderID, fragmentShaderID);
}

GLuint compileShader(const char *shaderPath, const GLuint shaderType) {
    GLuint shaderID = glCreateShader(shaderType);

    std::string shaderCode;
    std::ifstream shaderCodeStream(shaderPath, std::ios::in);
    if (shaderCodeStream.is_open()) {
        std::stringstream stringStream;
        stringStream << shaderCodeStream.rdbuf();
        shaderCode = stringStream.str();
        shaderCodeStream.close();
    } else {
        throw "Couldn't read shader.";
    }

    char const *sourcePointer = shaderCode.c_str();
    glShaderSource(shaderID, 1, &sourcePointer, nullptr);
    glCompileShader(shaderID);

    GLint isCompiled = 0;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(shaderID, maxLength, &maxLength, &errorLog[0]);
        printf("%s\n", &errorLog[0]);
        glDeleteShader(shaderID); // Don't leak the shader.
        throw "Couldn't compile shader.";
    }

    return shaderID;
}

GLuint linkProgram(const GLuint vertexShaderID, const GLuint fragmentShaderID) {
    GLuint programID = glCreateProgram();
    GLint status;

    for (auto shaderID : {vertexShaderID, fragmentShaderID}) {
        glAttachShader(programID, shaderID);
    }

    glLinkProgram(programID);
    glGetProgramiv(programID, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        int length = 0;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &length);
        if (length > 0) {
            GLchar infoLog[512];
            glGetProgramInfoLog(programID, 512, nullptr, infoLog);
            std::cerr << infoLog;
        }
        std::exit(1);
    }

    for (auto shaderID : {vertexShaderID, fragmentShaderID}) {
        glDetachShader(programID, shaderID);
        glDeleteShader(shaderID);
    }

    glValidateProgram(programID);
    glGetProgramiv(programID, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE) {
        int length = 0;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &length);
        if (length > 0) {
            GLchar infoLog[512];
            glGetProgramInfoLog(programID, 512, nullptr, infoLog);
            std::cerr << infoLog;
        }
        std::exit(1);
    }

    return programID;
}

void teardown(GLuint vertexbuffer, GLuint vertexArrayId, GLuint programID) {
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteVertexArrays(1, &vertexArrayId);
    glDeleteProgram(programID);
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}