//
// Created by Alexander Lyon on 2019-10-23.
//

#include "boids.hpp"
#include "../components/components.hpp"

#define N_NEAREST 20
#define MIN_DISTANCE 3

/**
 * Rule 1: Boids want to move towards the centre of mass of neighbouring boids.
 */
glm::vec3 rule1(entt::basic_view<entt::entity, position, velocity, fish> &sortedFish, entt::entity &ourEntity) {
    auto ourPosition = sortedFish.get<position>(ourEntity);

    uint8_t c = 0;
    glm::vec3 direction = {};
    for (auto& entity : sortedFish) {
        if (c == 0) { c++; continue; } // the fish should ignore itself
        if (sortedFish.get<fish>(entity).getGroup() != sortedFish.get<fish>(ourEntity).getGroup()) continue; // the fish should only group with its own group
        direction += sortedFish.get<position>(entity).position;
        if (++c == N_NEAREST) break;
    }

    // move 1% towards the center
    return (direction / (float)c - ourPosition.position) / 100.0f;
}

/**
 * Rule 2: Boids try to keep a small distance away from other objects (including other boids).
 *
 * todo(arlyon) smoothen out boids
 */
glm::vec3 rule2(entt::basic_view<entt::entity, position, velocity, fish> &sortedFish, entt::entity &ourEntity) {
    auto ourPosition = sortedFish.get<position>(ourEntity);

    uint8_t c = 0;
    glm::vec3 direction = {};
    for (auto& entity : sortedFish) {
        if (c == 0) { c++; continue; } // the fish should ignore itself
        auto entPos = sortedFish.get<position>(entity);
        auto distance = ourPosition.position - entPos.position;
        if (glm::length(distance) < MIN_DISTANCE) {
            direction += distance;
        }
        if (++c == N_NEAREST) break;
    }

    // move away from all nearby boids
    return direction;
}

/**
 * Rule 3: Boids try to match velocity with near boids.
 */
glm::vec3 rule3(entt::basic_view<entt::entity, position, velocity, fish> &sortedFish, entt::entity &ourEntity) {
    auto ourVelocity = sortedFish.get<velocity>(ourEntity);

    uint8_t c = 0;
    glm::vec3 perceived_velocity = {};
    for (auto& entity : sortedFish) {
        if (c == 0) { c++; continue; } // the fish should ignore itself
        perceived_velocity += sortedFish.get<velocity>(entity).velocity;
        if (++c == N_NEAREST) break;
    }

    return (perceived_velocity / (float)c - ourVelocity.velocity) / 8.0f;
}

/**
 * Makes the fish obey the flocking rules of Boids
 *
 * http://www.kfish.org/boids/pseudocode.html
 */
void boids(entt::registry &registry) {
    auto allEntities = registry.view<fish, velocity, position>();
    for (auto ourEntity : allEntities) {
        auto ourPosition = allEntities.get<position>(ourEntity);
        registry.sort<position>([ourPosition](auto &lhs, auto &rhs) {
            return (ourPosition.position - lhs.position).length() < (ourPosition.position - rhs.position).length();
        });
        auto sortedFish = registry.view<position, velocity, fish>();

        auto &ourVelocity = allEntities.get<velocity>(ourEntity);
        auto direction = rule1(sortedFish, ourEntity) + rule2(sortedFish, ourEntity) + rule3(sortedFish, ourEntity);
        ourVelocity.velocity += direction;
    }
}