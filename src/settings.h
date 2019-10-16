//
// Created by Alexander Lyon on 2019-10-16.
//

# pragma once

#include <stdint.h>

class Settings {
public:
    static Settings &getInstance() {
        static Settings instance;
        return instance;
    }

    Settings(Settings const &) = delete;
    void operator=(Settings const &) = delete;

    uint32_t fish = 32;
    bool enable_menu = false;
private:
    Settings() = default;
};
