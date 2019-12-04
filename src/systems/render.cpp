//
// Created by Alexander Lyon on 2019-10-11.
//

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

#include "../../lib/imgui_impl_opengl3.h"
#include "../../lib/imgui_impl_glfw.h"

#include "render.hpp"
#include "../components/components.hpp"
#include "../settings.hpp"

static double currentTime = 0;
int windowWidth = 640;
int windowHeight = 480;

void window_size_callback(GLFWwindow*, int width, int height) {
    windowWidth = width;
    windowHeight = height;
}

/**
 * Renders all models with positions from the
 * perspective of the provided camera entity.
 */
void render(entt::registry &registry, entt::entity *cam, double deltaTime) {
    auto &s = Settings::getInstance();

    currentTime += deltaTime * s.timeScale;

    auto color = s.color;
    glClearColor(color[0], color[1], color[2], 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto cameras = registry.view<camera, position>();
    camera camData = cameras.get<camera>(*cam);
    position camPos = cameras.get<position>(*cam);

    const glm::mat4 viewMatrix = glm::mat4_cast(camPos.orientation) * glm::translate(glm::mat4(1.0), -camPos.position);
    const glm::mat4 projectionMatrix = glm::perspective(
            glm::radians(*camData.fov),
            (float) windowWidth / windowHeight,
            0.1f,
            1000.0f
    );

    // render all the renderables
    auto renderableView = registry.view<renderable, position>();
    for (auto entity : renderableView) {
        auto objPos = renderableView.get<position>(entity);
        auto model = renderableView.get<renderable>(entity);
        model.render(objPos, camPos, projectionMatrix, viewMatrix, currentTime);
    }

    // render all the fish
    auto fishView = registry.view<renderable, position, fish>();

    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "Render Error: 0x" << std::hex << error << std::endl;
    }
}

void renderUI() {
    auto &settings = Settings::getInstance();

    /* DearImGui */
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Menu");
    ImGui::Text("Camera Settings");
    ImGui::SliderFloat("FOV", &settings.fov, 30.0f, 120.0f);
    ImGui::Separator();
    ImGui::Text("Scene Settings");
    ImGui::SliderInt("Fish Count", &settings.fish, 0, 1000);
    ImGui::ColorEdit3("Background Color", (float *) &settings.color);
    ImGui::SliderFloat("Time Scale", &settings.timeScale, 0.0f, 5.0f);
    ImGui::Text("Controls");
    if (ImGui::Button("Quit")) std::exit(0);
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
