#include <iostream>
#include <variant>

#include <GL/glew.h>
#include <imgui.h>

#include "render.h"
#include "update.h"
#include "initialize.h"

void initializeGeometry(GLuint &vertexBufferID, GLuint &vertexArrayID);

int main() {
    GLFWwindow *window = initializeOpenGL();
    initializeUI(window);

    GLuint vertexBufferID, vertexArrayID;
    initializeGeometry(vertexBufferID, vertexArrayID);

    GLuint shaderProgramID;
    try {
        shaderProgramID = initializeShaders("shaders/vertex_simple.glsl", "shaders/fragment_simple.glsl");
    } catch (const char *error) {
        std::cerr << error << std::endl;
        std::exit(1);
    }

    GLfloat currentTime;
    GLfloat deltaTime;
    GLfloat lastTime = 0.0;

    while (!glfwWindowShouldClose(window)) {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        update(currentTime);
        render(currentTime, vertexArrayID, shaderProgramID);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    teardown(vertexBufferID, vertexArrayID, shaderProgramID);
}

void initializeGeometry(GLuint &vertexBufferID, GLuint &vertexArrayID) {
    static const GLfloat g_vertex_buffer_data[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
    };
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
}

