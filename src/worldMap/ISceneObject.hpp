#pragma once

#include "Idrawable.hpp"
#include "Icontrollable.hpp"
#include "../transform.hpp"
#include "../assetManager/models/mesh.hpp"
#include "../collisionSystem.hpp"

class ISceneObject : public Icontrollable, public Idrawable {
protected:
    virtual void handeCollision(glm::vec3 mtv) = 0;
public:
    virtual void update() = 0;
    virtual ~ISceneObject() = default;
    virtual mesh& getMesh() = 0;
    virtual Transform& getTransform() = 0;
    virtual collisionMesh& getCollisionMesh() = 0;
    glm::vec3 checkCollision(const collisionMesh& otherCollisionMesh, const Transform& transform);
    void drawDebugAABB();
    
};
void drawDebugAABB_impl(collisionSystem::AABB box, glm::vec3 objectPos);
