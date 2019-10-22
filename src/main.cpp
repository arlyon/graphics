#include <iostream>
#include <variant>

#include <GL/glew.h>
#include <imgui.h>

#include "systems/render.h"
#include "systems/camera_orbit.h"
#include "initialize.h"
#include "settings.h"
#include "components/model.h"
#include "../lib/tiny_obj_loader.h"

bool convertGeometry(tinyobj::mesh_t mesh, const std::vector<tinyobj::real_t>& vertices, GLuint& vertexBufferID, GLuint& vertexArrayID);

int main() {
    auto &settings = Settings::getInstance();
    auto registry = entt::registry{};

    auto *window = initializeOpenGL();
    glfwSetWindowUserPointer(window, &settings); // add settings to window
    initializeUI(window);

    auto reader = tinyobj::ObjReader{};
	if (!reader.ParseFromFile("models/cube.obj")) {
		std::cout << "Couldn't load model." << std::endl;
		std::exit(1);
	}

    GLuint vertexBufferID, vertexArrayID;
	if (!convertGeometry(reader.GetShapes()[0].mesh, reader.GetAttrib().vertices, vertexBufferID, vertexArrayID)) {
	    std::cout << "Error parsing geometry. Are you only using triangles?" << std::endl;
	}

    GLuint shaderProgramID;
    try {
        shaderProgramID = initializeShaders("shaders/vertex_simple.glsl", "shaders/fragment_simple.glsl");
    } catch (const char *error) {
        std::cerr << error << std::endl;
        std::exit(1);
    }

    struct model fishModel = {vertexBufferID, vertexArrayID, shaderProgramID, reader.GetShapes()[0].mesh.num_face_vertices.size()};
    for (int i = 0; i < 3; i++) {
        auto fish = registry.create();
        registry.assign<position>(fish, i * 2, 0, 0);
        registry.assign<model>(fish, fishModel);
    }

	auto cam = registry.create();
	registry.assign<position>(cam, 4, 3, 3);
	registry.assign<camera>(cam, &settings.fov, window);

    GLfloat currentTime;
    GLfloat deltaTime;
    GLfloat lastTime = 0.0;

    while (!glfwWindowShouldClose(window)) {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        camera_orbit(registry, deltaTime);
        render(registry, &cam);
        if (settings.enable_menu) renderUI();
        glfwSwapBuffers(window);
    }

    teardown(vertexBufferID, vertexArrayID, shaderProgramID);
}

/**
 * Initializes a triangle into the given vertex buffer and array.
 * 
 * Creates a buffer and sets vertexBufferID to the new value.
 * Selects the bufferand loads the vertices into it.
 * 
 * @param vertexBufferID
 * @param vertexArrayID
 */
bool convertGeometry(tinyobj::mesh_t mesh, const std::vector<tinyobj::real_t> &vertices, GLuint &vertexBufferID, GLuint &vertexArrayID) {
    for (auto vert : mesh.num_face_vertices) {
        // only support triangles
        if (vert != 3) return false;
    }

	std::vector<tinyobj::real_t> vec;
	for (auto index : mesh.indices) {
		uint8_t vertexCount = 3;
		for (int i = 0; i < vertexCount; i++) {
			vec.push_back(vertices[index.vertex_index * vertexCount + i]);
		}
	}

    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tinyobj::real_t) * vec.size(), &vec.at(0), GL_STATIC_DRAW);

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    return true;
}

