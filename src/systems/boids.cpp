//
// Created by Alexander Lyon on 2019-10-23.
//

#include "boids.hpp"
#include "../components/components.hpp"
#include "../util/octree.hpp"

/**
 * Boids try to fly towards the centre of mass of neighbouring boids.
 *
 * todo(arlyon) Implement sight so that boids move where they can see
 */
glm::vec3 rule1(entt::registry &registry, entt::entity &entity, octree *tree) {
    auto ourFish = registry.view<fish, position>().get<position>(entity);
    registry.sort<position>([ourFish](auto &lhs, auto &rhs) {
        return (ourFish.position - lhs.position).length() < (ourFish.position - rhs.position).length();
    });

    auto sortedFish = registry.view<position, fish>();



    return glm::vec3{};
}

/**
 * Rule 2: Boids try to keep a small distance away from other objects (including other boids).
 */
glm::vec3 rule2(entt::registry &registry, entt::entity &entity, octree *tree) {
    return glm::vec3{};

}

/**
 * Rule 3: Boids try to match velocity with near boids.
 */
glm::vec3 rule3(entt::registry &registry, entt::entity &entity, octree *tree) {
    return glm::vec3{};

}

/**
 * Makes the fish obey the flocking rules of Boids
 *
 * http://www.kfish.org/boids/pseudocode.html
 */
void boids(entt::registry &registry) {
    auto allFish = registry.view<fish, velocity, position>();

    auto origin = glm::vec3();
    auto tree = new octree(&origin, 5000);
    for (auto entity : allFish) {
        tree->add(entity, allFish.get<position>(entity));
    }

    for (auto entity : allFish) {
        auto &vel = allFish.get<velocity>(entity);
        vel.velocity += rule1(registry, entity, tree) + rule2(registry, entity, tree) + rule3(registry, entity, tree);
    }
}