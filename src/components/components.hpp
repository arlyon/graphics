//
// Created by Alexander Lyon on 2019-10-21.
//

#pragma once

#include <stdint.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <GLFW/glfw3.h>

#include "../../lib/tiny_obj_loader.h"

/**
 * Some physical object with a position. Entities with a
 * position and a model are rendered to the screen.
 */
struct position {
    glm::vec3 position;
    glm::quat orientation;
};

/**
 * A model to be rendered by OpenGL
 */
class renderable {
    GLuint vertexBufferID; // the vertex buffer for this model
    GLuint vertexArrayID; // the vertex array for this model
    GLuint shaderProgramID; // the program to use when rendering this model
    GLuint textureID; // the texture ID to use when rendering this model
    uint64_t triangles; // the number of triangles
public:
    /**
    * Creates a renderable from a given obj, vertex shader, and fragment shader.
    * @param model The path to the model to use the renderable with.
    * @param vertex The path to the vertex shader to use.
    * @param fragment The path to the fragment shader to use.
    * @return A renderable.
    */
    renderable(const std::string &model, const std::string &vertex, const std::string &fragment);

    void render(position objPos, position camPos, const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix, float time);

    void close();
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
struct fish {
};

/**
 * A camera through which the world is rendered.
 */
struct camera {
    GLfloat *fov;
    GLFWwindow *window;
};
