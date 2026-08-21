#include "collisionSystem.hpp"

#include <glm/vec3.hpp>

using namespace collisionSystem;

/*collisionSystem::Result collisionSystem::checkMeshCollision(const AABB& boxA, const AABB& boxB)
{
    float deltaX1 = boxB.max.x - boxA.min.x;
    float deltaX2 = boxA.max.x - boxB.min.x;
    if (deltaX1 <= 0.0f || deltaX2 <= 0.0f) return {};

    float deltaY1 = boxB.max.y - boxA.min.y; 
    float deltaY2 = boxA.max.y - boxB.min.y; 
    if (deltaY1 <= 0.0f || deltaY2 <= 0.0f) return {};

    float deltaZ1 = boxB.max.z - boxA.min.z;
    float deltaZ2 = boxA.max.z - boxB.min.z; 
    if (deltaZ1 <= 0.0f || deltaZ2 <= 0.0f) return {};

    float overlapX = (deltaX1 < deltaX2) ? deltaX1 : -deltaX2;
    float overlapY = (deltaY1 < deltaY2) ? deltaY1 : -deltaY2;
    float overlapZ = (deltaZ1 < deltaZ2) ? deltaZ1 : -deltaZ2;

    float absX = std::abs(overlapX);
    float absY = std::abs(overlapY);
    float absZ = std::abs(overlapZ);

    if (absX <= absY && absX <= absZ) {
        return { true, {overlapX,0,0} };
    }
    if (absY <= absX && absY <= absZ) {
        return { true, {0,overlapY,0} };
    }
    return { true, {0,0,overlapZ} };
}*/

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
    if (boxA.max.y <= boxB.min.y || boxA.min.y >= boxB.max.y) return 0.0f;
    if (boxA.max.z <= boxB.min.z || boxA.min.z >= boxB.max.z) return 0.0f;

    return getAxisMtv(boxA.min.x, boxA.max.x, boxB.min.x, boxB.max.x);
}

float collisionSystem::checkCollisionY(const AABB& boxA, const AABB& boxB)
{
    if (boxA.max.x <= boxB.min.x || boxA.min.x >= boxB.max.x) return 0.0f;
    if (boxA.max.z <= boxB.min.z || boxA.min.z >= boxB.max.z) return 0.0f;

    return getAxisMtv(boxA.min.y, boxA.max.y, boxB.min.y, boxB.max.y);
}

float collisionSystem::checkCollisionZ(const AABB& boxA, const AABB& boxB)
{
    if (boxA.max.x <= boxB.min.x || boxA.min.x >= boxB.max.x) return 0.0f;
    if (boxA.max.y <= boxB.min.y || boxA.min.y >= boxB.max.y) return 0.0f;

    return getAxisMtv(boxA.min.z, boxA.max.z, boxB.min.z, boxB.max.z);
}
