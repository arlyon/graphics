//
// Created by Alexander Lyon on 2019-10-11.
//

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

#include "render.h"
#include "../../lib/imgui_impl_opengl3.h"
#include "../../lib/imgui_impl_glfw.h"
#include "../settings.h"

void render(entt::registry &registry, entt::entity *cam) {
    glClear(GL_COLOR_BUFFER_BIT);

	auto cameras = registry.view<camera, position>();
    camera camData = cameras.get<camera>(*cam);
    position camPos = cameras.get<position>(*cam);

	const glm::mat4 viewMatrix = glm::lookAt(
		camPos,
		glm::vec3(0, 0, 0), // looking at the origin
		glm::vec3(0, 1, 0)  // head is up
	);

	int width, height;
    glfwGetWindowSize(camData.window, &width, &height);
	const glm::mat4 projectionMatrix = glm::perspective(
		glm::radians(*camData.fov),
        (float)width / height,
		0.1f,
		100.0f
	);

    auto objects = registry.view<model, position>();
	for (auto object : objects) {
	    // todo(arlyon) instancing
		auto pos = objects.get<position>(object);
		auto mod = objects.get<model>(object);
        glUseProgram(mod.programID);
        glBindVertexArray(mod.vertexArrayID);

		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix[3][0] = pos.x;
		modelMatrix[3][1] = pos.y;
		modelMatrix[3][2] = pos.z;
		glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
		GLuint mvpID = glGetUniformLocation(mod.programID, "MVP");
		glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, mod.triangles * 3);

        glBindVertexArray(0);
        glUseProgram(0);
	}

    /* Error */
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Render Error: 0x";
        std::cout << std::hex << error << std::endl;
    }

    auto color = Settings::getInstance().color;
	glClearColor(color[0], color[1], color[2], 0.0f);
}

void renderUI() {
	auto& settings = Settings::getInstance();

    /* DearImGui */
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Debug Menu");
	ImGui::Checkbox("Camera Orbit", &settings.orbit);
	ImGui::SliderFloat("Camera FOV", &settings.fov, 30.0f, 120.0f);
	ImGui::SliderInt("Fish Count", &settings.fish, 1, 100);
    ImGui::ColorEdit3("Background Color", settings.color);
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}