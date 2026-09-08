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

enum class gameMode {
    SURVIVAL,
    CREATIVE,
    ADVENTURE,
    SPECTATOR
};


class Player : public ISceneObject {
private:
    Transform m_transform, m_lastState;
    glm::vec3 m_velocity{};
    gameMode m_mode{ gameMode::SURVIVAL };
    std::unique_ptr<basicModel> m_model;
    collisionMesh m_collisionMesh;

    const float m_speed = 0.5f;
    const float m_jumpForce = 8.4;
    const float m_eyeHeight = 1.62f;

    glm::vec3 m_bbMin{ -0.3f, 0.0f, -0.3f };
    glm::vec3 m_bbMax{ 0.3f, 1.8f, 0.3f };

    bool m_onGround = false;
    bool m_isSprinting = false;
    bool m_isFlying = false;

    int m_currentBLock = 1; // Temp

    void handleCollision(glm::vec3 mtv);
public:
    Player(glm::vec3 spawnPos = glm::vec3(0.0f, 16.0f, 0.0f));
    ~Player() override = default;

    void draw(float alpha) override;

    void MoveTo(glm::vec3 dest) override;
    void MoveOn(glm::vec3 delta) override;
    void Rotate(glm::vec3 deltaRotation) override;
    glm::vec3 getPos() override { return m_transform.pos; }
    glm::vec3 getVelocity() override { return m_velocity; }
    void setVelocity(glm::vec3 vel) override { m_velocity = vel; };
    bool isGrounded() const { return m_onGround; }
    bool isFlying() const { return m_isFlying; }
    void setFlying(bool flying) { m_isFlying = flying; }

    void update() override;

    mesh& getMesh() override {
        return m_model->getMesh();
    }

    Transform& getTransform() override {
        return m_transform;
    }

    collisionMesh& getCollisionMesh() {
        return m_collisionMesh;
    };

    void moveByKeyBoard(MoveDirection dir);
    void keyCallback();
    void mouseCallBack();

    void SaveStateForInterpolation() override {
        m_lastState = m_transform;
        if (m_model) {
            m_model->SaveStateForInterpolation();
        }
    }

};  