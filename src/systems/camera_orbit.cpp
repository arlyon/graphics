//
// Created by Alexander Lyon on 2019-10-11.
//

#include <GL/glew.h>

#include "camera_orbit.h"
#include "../components/components.h"
#include "../settings.h"

GLfloat rotateTime = 0.0f;

/**
 * Orbits the camera around the origin as long as
 * the orbit setting is set to true.
 */
void camera_orbit(entt::registry &registry, GLfloat deltaTime) {
    glfwPollEvents();

    auto &s = Settings::getInstance();
    if (!s.orbit) return;
    else rotateTime += deltaTime;

    auto cameras = registry.view<camera, position>();

    for (auto cam : cameras) {
        auto &pos = cameras.get<position>(cam);

        pos.position.x = 5 * glm::sin(rotateTime);
        pos.position.y = 5 * glm::cos(rotateTime);
    }
}