//
// Created by Alexander Lyon on 2019-10-11.
//

#pragma once

#include <entt/entt.hpp>
#include "../components/render.hpp"

extern int windowWidth;
extern int windowHeight;

void window_size_callback(GLFWwindow*, int width, int height);

void renderRenderables(entt::registry &registry, entt::entity *cam, double deltaTime);

void renderFish(entt::registry &registry, entt::entity *cam, double deltaTime, shader fishShader, renderable fishModel,
                GLuint modelBuffer, GLuint timeBuffer, GLuint hueBuffer);

void renderUI();

