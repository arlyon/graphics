//
// Created by Alexander Lyon on 2019-10-21.
//

#pragma once

#include <stdint.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

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
 * Some physical object with a position. Entities with a
 * position and a model are rendered to the screen.
 */
struct position {
    glm::vec3 position;
};

/**
 * Velocity. Entities with a velocity will have their
 * position updated by the physics engine.
 */
struct velocity {
    glm::vec3 velocity;
};

/**
 * A marker for the fish population manager to show
 * that the entity is a fish managed by it.
 */
struct fish {};

/**
 * A camera through which the world is rendered.
 */
struct camera {
    GLfloat *fov;
    GLFWwindow *window;
};