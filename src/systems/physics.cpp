//
// Created by Alexander Lyon on 2019-10-23.
//

#include <glm/gtc/quaternion.hpp>

#include "physics.hpp"
#include "../components/components.hpp"
#include "../settings.hpp"

#define DRAG 0.001f
#define TURN_SPEED 0.1f

static glm::vec3 forward = glm::vec3(0, 0, -1);

/**
 * Newton's First Law:
 *
 * Objects in motion stay in motion,
 * objects at rest, stay at rest.
 */
void physics(entt::registry &registry, double deltaTime) {
    auto &s = Settings::getInstance();

    auto view = registry.view<position, velocity>();
    for (auto entity : view) {
        position &pos = view.get<position>(entity);
        velocity &vel = view.get<velocity>(entity);
        pos.position += vel.velocity * (float) deltaTime * s.timeScale;

        // apply drag
        float dragForceMagnitude = pow(glm::length(vel.velocity), 2) * DRAG;
        if (dragForceMagnitude > 0) {
            auto dragForceVector = dragForceMagnitude * -glm::normalize(vel.velocity);
            vel.velocity += dragForceVector;
        }
    }
}

/**
 * Move the velocity and orientation toward one
 * another and accelerate in the orientation.
 */
void fish_physics(entt::registry &registry, float) {
    auto view = registry.view<position, velocity, fish>();
    for (auto entity : view) {
        position &pos = view.get<position>(entity);
        velocity &vel = view.get<velocity>(entity);
        vel.velocity = pos.orientation * forward * 5.0f;
    }
}