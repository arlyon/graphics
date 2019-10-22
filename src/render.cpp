//
// Created by Alexander Lyon on 2019-10-11.
//

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

#include "render.h"
#include "../lib/imgui_impl_opengl3.h"
#include "../lib/imgui_impl_glfw.h"
#include "settings.h"


const glm::mat4 PROJECTION = glm::perspective(glm::radians(60.0f), 1.4f, 0.1f, 100.0f);


void render(entt::registry &registry, entt::entity *cam, struct model fishModel) {
    glClear(GL_COLOR_BUFFER_BIT);

	auto cameras = registry.view<camera, position>();
    auto objects = registry.view<model, position>();
    auto objectCount = objects.size();

	const glm::mat4 view = glm::lookAt(
		cameras.get<position>(*cam),
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	const glm::mat4 proj = glm::perspective(
		glm::radians(*cameras.get<camera>(*cam).fov),
		1.4f, 
		0.1f, 
		100.0f
	);

	glUseProgram(fishModel.programID);
	glBindVertexArray(fishModel.vertexArrayID);
	for (auto model : objects) {
		auto pos = objects.get<position>(model);
		glm::mat4 Model = glm::mat4(pos.x);
		glm::mat4 mvp = proj * view * Model;
		GLuint MatrixID = glGetUniformLocation(fishModel.programID, "MVP");
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, fishModel.triangles * 3);
	}
	glBindVertexArray(0);
	glUseProgram(0);

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
    ImGui::ColorEdit3("Background Color", settings.color);
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}