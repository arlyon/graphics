//
// Created by Alexander Lyon on 2019-10-11.
//

#include <GL/glew.h>

#include "update.h"
#include "components/model.h"
#include "settings.h"

GLfloat rotateTime = 0.0f;

void update(entt::registry &registry, GLfloat deltaTime, GLfloat currentTime) {
    glfwPollEvents();

	auto &s = Settings::getInstance();
	if (!s.orbit) return;
	else rotateTime += deltaTime;

	auto cameras = registry.view<camera, position>();

	for (auto cam : cameras) {
		auto &pos = cameras.get<position>(cam);
		
		pos.x = 5 * glm::sin(rotateTime);
		pos.y = 5 * glm::cos(rotateTime);
	}
}