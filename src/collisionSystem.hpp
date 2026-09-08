#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <initializer_list>

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

	struct mesh {
	public:
		std::vector<AABB> boxes;
		AABB globalBounds;

		mesh() = default;
		mesh(std::initializer_list<AABB> init_list) 
			: boxes(init_list)
		{
			calculateGlobalBoxes();
		}

		operator const std::vector<AABB>& () const { return boxes; }
		operator std::vector<AABB>& () { return boxes; }

		auto begin() { return boxes.begin(); }
		auto end() { return boxes.end(); }

		auto begin() const { return boxes.begin(); }
		auto end() const  { return boxes.end(); }

		auto operator->() { return &boxes; }
		const auto* operator->() const { return &boxes; }
		auto& operator[](size_t index) { return boxes[index]; }
		const auto& operator[](size_t index) const { return boxes[index]; }

		void calculateGlobalBoxes() {
			for (auto& box : boxes) {
				globalBounds.min.x = std::min(box.min.x, globalBounds.min.x);
				globalBounds.min.y = std::min(box.min.y, globalBounds.min.y);
				globalBounds.min.z = std::min(box.min.z, globalBounds.min.z);

				globalBounds.max.x = std::max(box.max.x, globalBounds.max.x);
				globalBounds.max.y = std::max(box.max.y, globalBounds.max.y);
				globalBounds.max.z = std::max(box.max.z, globalBounds.max.z);
			}
		}
	};
}

using collisionMesh = collisionSystem::mesh;