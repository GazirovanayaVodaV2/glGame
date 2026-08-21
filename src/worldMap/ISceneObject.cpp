#include "ISceneObject.hpp"

#include "../assetManager/assetManager.hpp"
#include "../glfwContext.hpp"
#include "../camera.hpp"

glm::vec3 ISceneObject::checkCollision(const collisionMesh& otherCollisionMesh, const Transform& transform)
{
    //to do rewrite collisions
    glm::vec3 correction{};
    if (glm::dot(correction, correction) > 1e-6f) {
        handeCollision(correction);
        return correction;
    }

    return {};
}
void ISceneObject::drawDebugAABB()
{
    for (auto& meshes : getCollisionMesh()) {
        drawDebugAABB_impl(meshes, getPos());
    }
}

void drawDebugAABB_impl(collisionSystem::AABB box, glm::vec3 objectPos)
{
    glm::vec3 min = objectPos + box.min;
    glm::vec3 max = objectPos + box.max;

    glm::vec3 c[8] = {
        { min.x, min.y, min.z },
        { max.x, min.y, min.z },
        { max.x, min.y, max.z }, 
        { min.x, min.y, max.z }, 
        { min.x, max.y, min.z }, 
        { max.x, max.y, min.z }, 
        { max.x, max.y, max.z }, 
        { min.x, max.y, max.z }  
    };

    glm::vec3 lines[] = {
        c[0], c[1],  c[1], c[2],  c[2], c[3],  c[3], c[0],
        c[4], c[5],  c[5], c[6],  c[6], c[7],  c[7], c[4],
        c[0], c[4],  c[1], c[5],  c[2], c[6],  c[3], c[7]
    };

    auto& debugShader = mainAssetManager::get<shader>("debugShader");
    glfwContext::useShader(debugShader);
    debugShader.set<glm::mat4>("view", Camera::getView());
    debugShader.set<glm::mat4>("projection", glfwContext::projection);

    static GLuint vao = 0, vbo = 0;
    if (vao == 0) {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);
    }
    else {
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(lines), lines, GL_DYNAMIC_DRAW);

    glDisable(GL_DEPTH_TEST); 
    glDrawArrays(GL_LINES, 0, 24);
    glEnable(GL_DEPTH_TEST);

    glBindVertexArray(0);
}