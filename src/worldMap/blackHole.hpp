#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <openGLIncluder.hpp>

#include "transform.hpp"
#include "worldMap/ISceneObject.hpp"
#include "camera.hpp"
#include "assetManager/models/mesh.hpp"
#include "assetManager/models/basicModel.hpp"
#include "../worldMap/scene.hpp"
#include <collisionSystem.hpp>


class blackHole : public ISceneObject {
private:
    Transform m_transform, m_lastState;
    glm::vec3 m_velocity{},
        m_wantedVelocity{};
    std::unique_ptr<basicModel> m_model;
    collisionMesh m_collisionMesh{};
    double mass = 6.7 * pow(10, 26);

    FrameBuffer fbo{};

    void handleCollision(glm::vec3 mtv) {};
public:
    blackHole(glm::vec3 spawnPos = { 0,16,0 });
    ~blackHole() override = default;
    void draw(float alpha) override;

    void MoveTo(glm::vec3 dest) override;
    void MoveOn(glm::vec3 delta) override;
    void Rotate(glm::vec3 deltaRotation) override;
    glm::vec3 getPos() override { return m_transform.pos; }
    glm::vec3 getVelocity() override { return m_velocity; }
    void setVelocity(glm::vec3 vel) override { m_velocity = vel; };

    void update() override {};

    mesh& getMesh() override {
        return m_model->getMesh();
    }

    Transform& getTransform() override {
        return m_transform;
    }

    collisionMesh& getCollisionMesh() {
        return m_collisionMesh;
    };

    float getRadius() {
        constexpr double G = 6.67430e-11;
        constexpr double C = 299792458.0;
        return (float)((2 * G * mass) / pow(C, 2)) / (0.15f * 2.0f);
    }

    void SaveStateForInterpolation() override {
        m_lastState = m_transform;
        if (m_model) {
            m_model->SaveStateForInterpolation();
        }
    }
};