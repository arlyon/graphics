//
// Created by Alexander Lyon on 2019-10-23.
//

#include "physics.h"
#include "../components/model.h"

/**
 * Newton's First Law:
 *
 * Objects in motion stay in motion,
 * objects at rest, stay at rest.
 */
void physics(entt::registry &registry, float deltaTime) {
    auto view = registry.view<position, velocity>();
    for (auto entity : view) {
        position &pos = view.get<position>(entity);
        velocity &vel = view.get<velocity>(entity);
        pos.position += vel.velocity * deltaTime;
    }
}