#pragma once

#include <glm/vec3.hpp>
#include <glm/matrix.hpp>

struct Transform {
	glm::vec3 pos{}, rotation{}, scale = glm::vec3(1.0f);

	glm::mat4 getModel();
	glm::mat4 getInterpolated(Transform& lastState, float alpha);
};