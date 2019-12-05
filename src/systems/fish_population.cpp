//
// Created by Alexander Lyon on 2019-10-23.
//

#include <random>

#include <glm/gtc/quaternion.hpp>

#include "fish_population.hpp"
#include "../settings.hpp"
#include "../components/components.hpp"

#include "glm/gtc/quaternion.hpp"

static std::random_device rd;
static std::mt19937 eng(rd());
static std::uniform_real_distribution<float> dist(-1, 1);

#define SPAWN_LIMIT 1

/**
 * Handles the spawning and despawning of fish,
 * according to the value specified in the settings.
 *
 * Fish are spawned in and destroyed randomly.
 */
void fish_population(entt::registry &registry) {
    auto &s = Settings::getInstance();

    auto fishView = registry.view<fish, position>();
    int64_t fishDeficit = s.fish - fishView.size();
    if (fishDeficit >= 0) {
        // create some (or none)
        for (int i = 0; i < fishDeficit && i < SPAWN_LIMIT; i++) {
            auto entity = registry.create();
            registry.assign<position>(entity, glm::vec3(0, 1.5f + dist(eng), -8.0f + dist(eng)), glm::quatLookAt(glm::normalize(glm::vec3(-5.0f, dist(eng), dist(eng))), glm::vec3(0, 1, 0)));
            registry.assign<velocity>(entity, glm::vec3(0, 0, 0));
            registry.assign<fish>(entity, (uint8_t)(s.fish - fishDeficit + i) % 5);
        }
    } else {
        // kill some
        for (auto fish : fishView) {
            registry.destroy(fish);
            if (++fishDeficit == 0) break;
        }
    }
}
