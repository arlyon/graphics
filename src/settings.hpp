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

    int32_t fish = 1;
	float fov = 60.0f;
	float color[3] = {0.4f, 0.8f, 1.0f};

    bool enable_menu = true;
	bool orbit = false;
private:
    Settings() = default;
};
