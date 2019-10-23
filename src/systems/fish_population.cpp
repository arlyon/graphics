//
// Created by Alexander Lyon on 2019-10-23.
//

#include "fish_population.h"
#include "../settings.h"
#include "../components/model.h"

/**
 * Handles the spawning and despawning of fish,
 * according to the value specified in the settings.
 *
 * Fish are spawned in and destroyed randomly.
 */
void fish_population(entt::registry &registry, model fishModel) {
    auto &s = Settings::getInstance();

    auto f = registry.view<fish, model, position>();
    int64_t fishDeficit = s.fish - f.size();
    if (fishDeficit >= 0) {
        // create some (or none)
        for (int i = s.fish; i < s.fish + fishDeficit; i++) {
            auto entity = registry.create();
            registry.assign<position>(entity, glm::vec3(i, 0, 0));
            registry.assign<velocity>(entity, glm::vec3(0.5, 0, 0));
            registry.assign<model>(entity, fishModel);
            registry.assign<fish>(entity);
        }
    } else {
        // kill some
        auto fishIter = f.begin();
        for (int i = 0; i < -fishDeficit; i++) {
            auto entity = std::next(fishIter);
            registry.destroy(*entity);
        }
    }
}