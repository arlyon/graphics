//
// Created by Alexander Lyon on 2019-10-16.
//

# pragma once

#include <stdint.h>

#include <glm/glm.hpp>

class Settings {
public:
    static Settings &getInstance() {
        static Settings instance;
        return instance;
    }

    Settings(Settings const &) = delete;

    void operator=(Settings const &) = delete;

    bool enable_menu = false;

    // camera
    float fov = 80.0f;
	float mouse_sensitivity = 0.005f;

    // scene
    int fish = 50;
    glm::vec3 color = glm::vec3(0.1f, 0.12f, 0.33f);
    float time_scale = 1.0f;

    // boids
    int group_size = 10;
    int boid_avoid = 10;
    float min_boid_distance = 10;
    float min_camera_distance = 10;

private:
    Settings() = default;
};
