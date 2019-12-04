//
// Created by Alexander Lyon on 2019-10-23.
//

#include "boids.hpp"
#include "../components/components.hpp"
#include "../settings.hpp"

#define N_NEAREST 20
#define MIN_BOID_DISTANCE 5
#define MIN_AVOID_DISTANCE 10

/**
 * Rule 1: Boids want to move towards the centre of mass of neighbouring boids.
 */
glm::vec3 rule1(entt::basic_view<entt::entity, position, velocity, fish> &sortedFish, entt::entity &ourEntity) {
    auto ourPosition = sortedFish.get<position>(ourEntity);

    uint8_t c = 0;
    glm::vec3 direction = {};
    for (auto &entity : sortedFish) {
        if (entity == ourEntity) { continue;} // the fish should ignore itself
        if (sortedFish.get<fish>(entity).getGroup() != sortedFish.get<fish>(ourEntity).getGroup()) continue; // the fish should only group with its own group
        direction += sortedFish.get<position>(entity).position;
        if (++c == N_NEAREST) break;
    }

    return direction / (float) c - ourPosition.position;
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
    for (auto &entity : sortedFish) {
        if (entity == ourEntity) { continue; } // the fish should ignore itself
        auto entPos = sortedFish.get<position>(entity);
        auto gap = entPos.position - ourPosition.position;
        auto distance = glm::length(gap);
        if (distance > MIN_BOID_DISTANCE) continue;

        direction -= glm::normalize(gap) * ((float)MIN_BOID_DISTANCE - distance);
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
    for (auto &entity : sortedFish) {
        if (c == 0) {
            c++;
            continue;
        } // the fish should ignore itself
        perceived_velocity += sortedFish.get<velocity>(entity).velocity;
        if (++c == N_NEAREST) break;
    }

    return (perceived_velocity / (float) c - ourVelocity.velocity) / 8.0f;
}

/**
 * Additional Rule 4: Boids try to move toward the origin.
 */
glm::vec3 rule4(entt::basic_view<entt::entity, position, velocity, fish> &sortedFish, entt::entity &ourEntity) {
    return -sortedFish.get<position>(ourEntity).position;
}

/**
 * Additional Rule 5: Boids try to avoid the camera.
 */
glm::vec3 rule5(entt::basic_view<entt::entity, position, velocity, fish> &sortedFish, entt::entity &ourEntity,
                entt::registry &registry, entt::entity *avoid) {
    auto ourPosition = sortedFish.get<position>(ourEntity);
    auto avoidPos = registry.get<position>(*avoid);

    auto gap = ourPosition.position - avoidPos.position;
    auto distance = glm::length(gap);
    if (distance > MIN_AVOID_DISTANCE) return glm::vec3{};
    else return gap * ((float)MIN_AVOID_DISTANCE - distance);
}

/**
 * Makes the fish obey the flocking rules of Boids
 *
 * http://www.kfish.org/boids/pseudocode.html
 */
void boids(entt::registry &registry, entt::entity *avoid, double deltaTime) {
    auto &s = Settings::getInstance();

    auto allEntities = registry.view<fish, velocity, position>();
    for (auto ourEntity : allEntities) {
        auto &ourPosition = allEntities.get<position>(ourEntity);
        registry.sort<position>([ourPosition](auto &lhs, auto &rhs) {
            return (ourPosition.position - lhs.position).length() < (ourPosition.position - rhs.position).length();
        });
        auto sortedFish = registry.view<position, velocity, fish>();

        glm::vec3 direction = {};
        direction += rule1(sortedFish, ourEntity);
        direction += rule2(sortedFish, ourEntity);
        // direction += rule3(sortedFish, ourEntity); // todo
        direction += rule4(sortedFish, ourEntity);
        direction += rule5(sortedFish, ourEntity, registry, avoid);

        if (avoid != nullptr) {
            direction += rule5(sortedFish, ourEntity, registry, avoid);
        }

        if (glm::length(direction) > 0.01f) {
            auto targetOrientation = glm::quatLookAt(glm::normalize(direction), glm::vec3(0, 1, 0));
            auto newOrientation = glm::slerp(ourPosition.orientation, targetOrientation, 0.4f * (float)deltaTime * s.timeScale);
            ourPosition.orientation = newOrientation;
        }
    }
}