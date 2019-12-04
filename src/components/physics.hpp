//
// Created by Alexander Lyon on 2019-12-04.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


/**
 * Some physical object with a position. Entities with a
 * position and a model are rendered to the screen.
 */
struct position {
    glm::vec3 position;
    glm::quat orientation;
};

/**
 * Velocity. Entities with a velocity will have their
 * position updated by the physics engine.
 */
struct velocity {
    glm::vec3 velocity;
};
