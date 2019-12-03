//
// Created by Alexander Lyon on 2019-10-16.
//

# pragma once

#include <stdint.h>

#include <GL/glew.h>
#include <glm/glm.hpp>

class Settings {
public:
    static Settings &getInstance() {
        static Settings instance;
        return instance;
    }

    Settings(Settings const &) = delete;

    void operator=(Settings const &) = delete;

    int32_t fish = 30;
    float fov = 60.0f;
    glm::vec3 color = glm::vec3(0.1f, 0.12f, 0.33f);

    float timeScale = 1.0f;

    bool enable_menu = false;
	float mouse_sensitivity = 0.005f;
private:
    Settings() = default;
};
