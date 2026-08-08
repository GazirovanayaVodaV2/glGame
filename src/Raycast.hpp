#pragma once
#include "glm/vec3.hpp"
#include "worldMap/scene.hpp"
struct RaycastHit {
	enum class HitType {
		None,
		Block,
		Entity
	} type = HitType::None;

	glm::ivec3 blockPos = glm::ivec3(0);
	glm::ivec3 adjacentBlockPos = glm::ivec3(0);
	glm::vec3 normal = glm::vec3(0);
	int blockId = 0;
	float distance = 0.0f;
};

RaycastHit raycastVoxels(dimensionBase* world, const glm::vec3 origin, const glm::vec3 direction, float maxDistance);