//
// Created by Alexander Lyon on 2019-10-11.
//

#include <GLFW/glfw3.h>

#include "update.h"

void update(ECS::World *world, GLfloat deltaTime) {
    glfwPollEvents();
    world->tick(deltaTime);


}