#include "Raycast.hpp"

RaycastHit raycastVoxels(dimensionBase* world, const glm::vec3 origin, const glm::vec3 direction, float maxDistance)
{
    RaycastHit result;

    // Вектор направления должен быть нормализован
    glm::vec3 rayDir = glm::normalize(direction);
    // Текущий воксель
    int mapX = static_cast<int>(std::floor(origin.x));
    int mapY = static_cast<int>(std::floor(origin.y));
    int mapZ = static_cast<int>(std::floor(origin.z));

    // Направление шага (+1 или -1)
    int stepX = (rayDir.x > 0) ? 1 : -1;
    int stepY = (rayDir.y > 0) ? 1 : -1;
    int stepZ = (rayDir.z > 0) ? 1 : -1;

    // Расстояние вдоль луча для прохождения одного целого блока по осям
    glm::vec3 deltaDist;
    deltaDist.x = (std::abs(rayDir.x) < 1e-6f) ? 1e30f : std::abs(1.0f / rayDir.x);
    deltaDist.y = (std::abs(rayDir.y) < 1e-6f) ? 1e30f : std::abs(1.0f / rayDir.y);
    deltaDist.z = (std::abs(rayDir.z) < 1e-6f) ? 1e30f : std::abs(1.0f / rayDir.z);

    // Дистанция до первой грани вокселя
    glm::vec3 sideDist;
    sideDist.x = (stepX > 0) ? (mapX + 1.0f - origin.x) * deltaDist.x : (origin.x - mapX) * deltaDist.x;
    sideDist.y = (stepY > 0) ? (mapY + 1.0f - origin.y) * deltaDist.y : (origin.y - mapY) * deltaDist.y;
    sideDist.z = (stepZ > 0) ? (mapZ + 1.0f - origin.z) * deltaDist.z : (origin.z - mapZ) * deltaDist.z;

    glm::ivec3 hitNormal(0);
    float currentDist = 0.0f;

    while (currentDist < maxDistance) {
        // Проверяем блок в текущих координатах
        int blockId = world->getBlock(mapX, mapY, mapZ);
        if (blockId > 0) { // 0 — воздух, >0 — твердый блок
            result.type = RaycastHit::HitType::Block;
            result.blockPos = { mapX, mapY, mapZ };
            result.normal = glm::vec3(hitNormal);
            result.adjacentBlockPos = result.blockPos + hitNormal; // Смежная позиция для постройки
            result.distance = currentDist;
            result.blockId = blockId;
            return result;
        }

        // Переходим к следующей грани воксельной сетки
        if (sideDist.x < sideDist.y) {
            if (sideDist.x < sideDist.z) {
                currentDist = sideDist.x;
                sideDist.x += deltaDist.x;
                mapX += stepX;
                hitNormal = glm::ivec3(-stepX, 0, 0);
            }
            else {
                currentDist = sideDist.z;
                sideDist.z += deltaDist.z;
                mapZ += stepZ;
                hitNormal = glm::ivec3(0, 0, -stepZ);
            }
        }
        else {
            if (sideDist.y < sideDist.z) {
                currentDist = sideDist.y;
                sideDist.y += deltaDist.y;
                mapY += stepY;
                hitNormal = glm::ivec3(0, -stepY, 0);
            }
            else {
                currentDist = sideDist.z;
                sideDist.z += deltaDist.z;
                mapZ += stepZ;
                hitNormal = glm::ivec3(0, 0, -stepZ);
            }
        }
    }

    return result;
}