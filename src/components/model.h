//
// Created by Alexander Lyon on 2019-10-21.
//

#pragma once

#include <stdint.h>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "../../lib/tiny_obj_loader.h"

/**
 * A model to be rendered by OpenGL
 */
struct model {
    GLuint vertexBufferID; // the vertex buffer for this model
    GLuint vertexArrayID; // the vertex array for this model
    GLuint programID; // the program to use when rendering this model
    uint64_t triangles; // the number of triangles
};

/**
 *
 */
typedef glm::vec3 position;

struct instance;

struct camera {
	GLfloat *fov;
	GLFWwindow *window;
};