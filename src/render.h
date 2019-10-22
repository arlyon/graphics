//
// Created by Alexander Lyon on 2019-10-11.
//

#pragma once

#include <GLFW/glfw3.h>
#include <entt/entt.hpp>

#include "components/model.h"

void render(entt::registry &registry, entt::entity *cam);
void renderUI();

