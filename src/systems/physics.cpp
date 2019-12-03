//
// Created by Alexander Lyon on 2019-10-23.
//

#include <glm/gtc/quaternion.hpp>

#include "physics.hpp"
#include "../components/components.hpp"
#include "../settings.hpp"

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
        pos.position += vel.velocity * (float)deltaTime * s.timeScale;
    }
}