#include "collisionSystem.hpp"

#include <glm/vec3.hpp>

using namespace collisionSystem;

static inline float getAxisMtv(float aMin, float aMax, float bMin, float bMax)
{
    const float overlap = std::min(aMax, bMax) - std::max(aMin, bMin);
    if (overlap <= 0.0f) {
        return 0.0f;
    }

    const float centerA = (aMin + aMax) * 0.5f;
    const float centerB = (bMin + bMax) * 0.5f;

    return (centerA < centerB) ? -overlap : overlap;
}


float collisionSystem::checkCollisionX(const AABB& boxA, const AABB& boxB)
{
    if (boxA.max.y < boxB.min.y || boxA.min.y > boxB.max.y) return 0.0f;
    if (boxA.max.z < boxB.min.z || boxA.min.z > boxB.max.z) return 0.0f;

    return getAxisMtv(boxA.min.x, boxA.max.x, boxB.min.x, boxB.max.x);
}

float collisionSystem::checkCollisionY(const AABB& boxA, const AABB& boxB)
{
    if (boxA.max.x < boxB.min.x || boxA.min.x > boxB.max.x) return 0.0f;
    if (boxA.max.z < boxB.min.z || boxA.min.z > boxB.max.z) return 0.0f;

    return getAxisMtv(boxA.min.y, boxA.max.y, boxB.min.y, boxB.max.y);
}

float collisionSystem::checkCollisionZ(const AABB& boxA, const AABB& boxB)
{
    if (boxA.max.x < boxB.min.x || boxA.min.x > boxB.max.x) return 0.0f;
    if (boxA.max.y < boxB.min.y || boxA.min.y > boxB.max.y) return 0.0f;

    return getAxisMtv(boxA.min.z, boxA.max.z, boxB.min.z, boxB.max.z);
}
