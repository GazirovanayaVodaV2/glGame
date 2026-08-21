#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>

namespace collisionSystem {
	struct AABB {
		glm::vec3 min{std::numeric_limits<float>::max()};
		glm::vec3 max{std::numeric_limits<float>::lowest()};

		AABB offset(const glm::vec3 pos) const {
			return { min + pos, max + pos };
		}
	};

	struct Result {
		bool isColliding = false;
		glm::vec3 mtv{};
	};

	float checkCollisionY(const AABB& boxA, const AABB& boxB);
	float checkCollisionX(const AABB& boxA, const AABB& boxB);
	float checkCollisionZ(const AABB& boxA, const AABB& boxB);
}

using collisionMesh = std::vector<collisionSystem::AABB>;