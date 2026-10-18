#pragma once
#include <vector>
#include <glm/vec3.hpp>

namespace lightSystem {
	struct point {
		static constexpr int maxCount = 64;

		glm::vec3 m_pos{};
		glm::ivec3 m_color{ 255, 255, 255 };
		float m_radius = 1.0f, m_power = 0.0f;
	};

	struct arrayOfLights {
		std::vector<point> points;
	};
}
