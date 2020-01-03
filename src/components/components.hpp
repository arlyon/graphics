//
// Created by Alexander Lyon on 2019-10-21.
//

#pragma once

#include <stdint.h>

#include <GLFW/glfw3.h>

#include "render.hpp"
#include "../../lib/tiny_obj_loader.h"

/**
 * A marker for the fish population manager to show
 * that the entity is a fish managed by it.
 */
class fish {
    uint8_t group;
    float timeOffset;
    float hueShift;
public:
    fish(uint8_t group);

    float getHueShift() const { return this->hueShift; }

    uint8_t getGroup() const { return this->group; }

    float getTimeOffset() const { return this->timeOffset; }
};

/**
 * A camera through which the world is rendered.
 */
struct camera {
    GLfloat *fov;
    GLFWwindow *window;
};
