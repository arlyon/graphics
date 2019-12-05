//
// Created by Alexander Lyon on 2019-10-23.
//

#include "boids.hpp"
#include "../components/components.hpp"
#include "../settings.hpp"

const Settings &s = Settings::getInstance();

/**
 * Rule 1: Boids want to move towards the centre of mass of neighbouring boids.
 */
glm::vec3 rule1(entt::view<entt::exclude_t<>, position, fish> &sortedFish, entt::entity &ourEntity) {
    auto ourPosition = sortedFish.get<position>(ourEntity);

    uint8_t grouped_with = 0;
    glm::vec3 direction = {};
    for (auto &entity : sortedFish) {
        if (entity == ourEntity) { continue;} // the fish should ignore itself
        if (sortedFish.get<fish>(entity).getGroup() != sortedFish.get<fish>(ourEntity).getGroup()) continue; // the fish should only group with its own group
        direction += sortedFish.get<position>(entity).position;
        if (++grouped_with == s.group_size) break;
    }

    return direction / (float) grouped_with - ourPosition.position;
}

/**
 * Rule 2: Boids try to keep a small distance away from other objects (including other boids).
 */
glm::vec3 rule2(entt::view<entt::exclude_t<>, position, fish> &sortedFish, entt::entity &ourEntity) {
    auto ourPosition = sortedFish.get<position>(ourEntity);

    uint8_t avoided = 0;
    glm::vec3 direction = {};
    for (auto &entity : sortedFish) {
        if (entity == ourEntity) { continue; } // the fish should ignore itself
        auto entPos = sortedFish.get<position>(entity);
        auto gap = entPos.position - ourPosition.position;
        auto distance = glm::length(gap);
        if (distance > s.min_boid_distance) continue;
        direction -= (gap / distance) * (s.min_boid_distance - distance);
        if (++avoided == s.boid_avoid) break;
    }

    return direction;
}

/**
 * Additional Rule 4: Boids try to move toward the origin.
 */
glm::vec3 rule4(entt::view<entt::exclude_t<>, position, fish> &sortedFish, entt::entity &ourEntity) {
    return -sortedFish.get<position>(ourEntity).position;
}

/**
 * Additional Rule 5: Boids try to avoid the camera.
 */
glm::vec3 rule5(entt::view<entt::exclude_t<>, position, fish> &sortedFish, entt::entity &ourEntity,
                entt::registry &registry, entt::entity *avoid) {
    auto ourPosition = sortedFish.get<position>(ourEntity);
    auto avoidPos = registry.get<position>(*avoid);

    auto gap = ourPosition.position - avoidPos.position;
    auto distance = glm::length(gap);
    if (distance > s.min_camera_distance) return glm::vec3{};
    else return gap * (s.min_camera_distance - distance);
}

/**
 * Makes the fish obey the flocking rules of Boids
 *
 * http://www.kfish.org/boids/pseudocode.html
 */
void boids(entt::registry &registry, entt::entity *avoid, double deltaTime) {
    auto allEntities = registry.view<fish, position>();
    for (auto ourEntity : allEntities) {
        auto &ourPosition = allEntities.get<position>(ourEntity);
        registry.sort<position>([ourPosition](auto &lhs, auto &rhs) {
            return (ourPosition.position - lhs.position).length() < (ourPosition.position - rhs.position).length();
        }, entt::insertion_sort());
        registry.sort<fish, position>();
        auto sortedFish = registry.view<position, fish>();

        glm::vec3 direction = {};
        direction += rule1(sortedFish, ourEntity);
        direction += rule2(sortedFish, ourEntity);
        direction += rule4(sortedFish, ourEntity);
        if (avoid != nullptr) direction += rule5(sortedFish, ourEntity, registry, avoid);

        if (glm::length(direction) > 0.01f) {
            auto targetOrientation = glm::quatLookAt(glm::normalize(direction), glm::vec3(0, 1, 0));
            auto newOrientation = glm::slerp(ourPosition.orientation, targetOrientation, 0.4f * (float)deltaTime * s.time_scale);
            ourPosition.orientation = newOrientation;
        }
    }
}