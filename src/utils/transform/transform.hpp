#pragma once

#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/quaternion.hpp>

struct Transform {
	glm::vec3 pos{}, scale = glm::vec3(1.0f);
	glm::quat orientation{1.0f,0.0f,0.0f,0.0f};

	glm::mat4 getModel();
	glm::mat4 getInterpolated(Transform& lastState, float alpha);

	void rotate(glm::vec3 degrees);
	glm::vec3 getRotation();
};