//
// Created by Alexander Lyon on 2019-10-11.
//

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../../lib/imgui_impl_opengl3.h"
#include "../../lib/imgui_impl_glfw.h"

#include "render.hpp"
#include "../components/components.hpp"
#include "../settings.hpp"

static double currentTime = 0;
int windowWidth = 1280;
int windowHeight = 720;

void window_size_callback(GLFWwindow*, int width, int height) {
    windowWidth = width;
    windowHeight = height;
}

/**
 * Renders all models with positions from the
 * perspective of the provided camera entity.
 */
void renderRenderables(entt::registry &registry, entt::entity *cam, double deltaTime) {
    auto &s = Settings::getInstance();
    currentTime += deltaTime * s.timeScale;

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
        auto [objPos, model] = renderableView.get<position, renderable>(entity);
        model.render(objPos, camPos, projectionMatrix, viewMatrix, currentTime);
    }
}

static size_t fishCount = 0;

void renderFish(entt::registry &registry, entt::entity *cam, shader fishShader, renderable fishModel, GLuint modelBuffer,
                GLuint timeBuffer, GLuint hueBuffer) {
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

    const glm::mat4 projectionViewMatrix = projectionMatrix * viewMatrix;

    // batch all per-object data for calculation on the shader
    auto fishView = registry.view<fish, position>();
    std::vector<glm::mat4> modelMatrices;
    std::vector<float> hueOffset;
    std::vector<float> timeOffset;
    modelMatrices.reserve(fishView.size());
    hueOffset.reserve(fishView.size());
    timeOffset.reserve(fishView.size());
    for (entt::entity entity : fishView) {
        auto [pos, f] = fishView.get<position, fish>(entity);
        modelMatrices.push_back(glm::translate(glm::mat4(1.0f), pos.position) * glm::mat4_cast(pos.orientation));
        hueOffset.push_back(f.getHueShift());
        timeOffset.push_back(f.getTimeOffset());
    }

    // matrices will change often, so dynamic draw and sub every frame the fish size doesnt change
    glBindBuffer(GL_ARRAY_BUFFER, modelBuffer);
    if (fishView.size() != fishCount) {
        glBufferData(GL_ARRAY_BUFFER, fishView.size() * sizeof(glm::mat4), glm::value_ptr(modelMatrices.front()), GL_DYNAMIC_DRAW);
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, fishView.size() * sizeof(glm::mat4), glm::value_ptr(modelMatrices.front()));
    }

    // time and hue offset are unique to the fish and will not change unless the fishView changes, so static draw
    if (fishView.size() != fishCount) {
        glBindBuffer(GL_ARRAY_BUFFER, timeBuffer);
        glBufferData(GL_ARRAY_BUFFER, fishView.size() * sizeof(float), &timeOffset.front(), GL_STATIC_DRAW);
    }

    if (fishView.size() != fishCount) {
        glBindBuffer(GL_ARRAY_BUFFER, hueBuffer);
        glBufferData(GL_ARRAY_BUFFER, fishView.size() * sizeof(float), &hueOffset.front(), GL_STATIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    fishShader.use();
    fishShader.setMatrix("projectionViewMatrix", projectionViewMatrix);
    fishShader.setFloat("time", (float) currentTime);
    fishShader.setVector("cameraPos", camPos.position);
    fishShader.prepareTextures();
    fishModel.setTextures();
    fishModel.draw(fishView.size());

    fishCount = fishView.size();
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
