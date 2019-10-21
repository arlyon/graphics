//
// Created by Alexander Lyon on 2019-10-16.
//

# pragma once

#include <stdint.h>

#include <GL/glew.h>

class Settings {
public:
    static Settings &getInstance() {
        static Settings instance;
        return instance;
    }

    Settings(Settings const &) = delete;
    void operator=(Settings const &) = delete;

    uint32_t fish = 32;
	GLfloat fov = 60.0f;
    bool enable_menu = true;
	bool orbit = true;
private:
    Settings() = default;
};
