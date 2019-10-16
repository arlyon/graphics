//
// Created by Alexander Lyon on 2019-10-11.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <iostream>

#include "render.h"
#include "../lib/imgui_impl_opengl3.h"
#include "../lib/imgui_impl_glfw.h"

void render(GLfloat time, GLuint vertexArrayID, GLuint shaderProgramID) {
    glClear(GL_COLOR_BUFFER_BIT);

    /* Draw Triangle */
    glUseProgram(shaderProgramID);
    glBindVertexArray(vertexArrayID);
    glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle

    /* DearImGui */
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Demo window");
    ImGui::Button("Hello!");
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    /* Error */
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Render Error: 0x";
        std::cout << std::hex << error << std::endl;
    }
}