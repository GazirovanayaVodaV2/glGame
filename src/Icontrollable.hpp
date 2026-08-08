#pragma once

#include <glm/vec3.hpp>

class Icontrollable {
public:
	virtual void MoveTo(glm::vec3 dest) {};
	virtual void MoveOn(glm::vec3 delta) {};
	virtual void Rotate(glm::vec3 deltaRotation) {};
};