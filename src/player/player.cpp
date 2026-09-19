#include <algorithm>
#include <iostream>
#include <functional>
#include "player.hpp"
#include "timer.hpp"

#include "../glfwContext.hpp"

#include "../Raycast.hpp"

void Player::handleCollision(glm::vec3 mtv)
{
    MoveOn(mtv);
    if (mtv.x != 0.0f) m_velocity.x = 0.0f;
    if (mtv.y != 0.0f) m_velocity.y = 0.0f;
    if (mtv.z != 0.0f) m_velocity.z = 0.0f;

    if (mtv.y > 0.0f) {
        m_onGround = mtv.y > 0.0f;
    }
    
}

Player::Player(glm::vec3 spawnPos)
: m_collisionMesh({{{-0.3f, 0.0f, -0.3f}, {0.3f, 1.8f, 0.3f}}})
{
    m_model = std::make_unique<basicModel>("steve", "steve", "blockShader");
    m_model->Scale({0.5,0.5,0.5});
    m_model->MoveTo(spawnPos);

    glfwContext::addKeyboardInputEvent(this, &Player::keyCallback);
    glfwContext::addMouseInputEvent(this, &Player::mouseCallBack);

    Camera::connectToObject(this);
    Camera::setOffset({0.0, 1.6f, 0.0f});

    MoveTo(spawnPos);
}

void Player::draw(float alpha) {
    if (m_mode != gameMode::SPECTATOR && m_model) {
        //m_model->draw(alpha); //Model goin crazy so i fix this later
    }
}

void Player::update() {
    /*
    
    Rotate player model
    
    */
    //std::cout << getPos().y << std::endl;

    auto cameraFront = Camera::getFront();
    auto cameraFrontXZ = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
    //float targetYaw = glm::degrees(glm::atan(cameraFrontXZ.x, cameraFrontXZ.z));
    float targetYaw = (glm::atan(cameraFrontXZ.x, cameraFrontXZ.z));
    float deltaYaw = targetYaw - m_transform.rotation.y;
    Rotate(glm::vec3(0.0f, deltaYaw, 0.0f));


    
    float friction = m_onGround ? 0.6f : 0.9f;
    m_velocity.x = m_velocity.x * friction + m_wantedVelocity.x * (1.0f - friction);
    m_velocity.z = m_velocity.z * friction + m_wantedVelocity.z * (1.0f - friction);

    m_velocity.y = (m_velocity.y - worldConstants::gravity) * 0.98f;
}

//to do fix input
void Player::keyCallback(GLFWwindow* window)
{
    float yawRad = m_transform.rotation.y;
    float pitchRad = m_transform.rotation.x;

    glm::vec3 front;
    front.x = std::sin(yawRad) * std::cos(pitchRad);
    front.y = std::sin(pitchRad);
    front.z = std::cos(yawRad) * std::cos(pitchRad);
    front = glm::normalize(front);

    const glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 moveFront;
    glm::vec3 right;

    if (m_isFlying) {
        glm::vec3 front;
        front.x = std::sin(yawRad) * std::cos(pitchRad);
        front.y = std::sin(pitchRad);
        front.z = std::cos(yawRad) * std::cos(pitchRad);
        front = glm::normalize(front);

        const glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        right = glm::normalize(glm::cross(front, worldUp));
        moveFront = front;
    }
    else {
        moveFront = glm::vec3(std::sin(yawRad), 0.0f, std::cos(yawRad));
        right = glm::vec3(std::cos(yawRad), 0.0f, -std::sin(yawRad));
    }

    glm::vec3 moveDir{ 0.0f };
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) moveDir += moveFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) moveDir -= moveFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) moveDir += right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) moveDir -= right;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (m_onGround) {
            m_velocity.y = m_jumpForce; 
            m_onGround = false;
        }
    }

    float currentSpeed = m_speed * (m_isSprinting ? 1.5f : 1.0f);
    if (glm::length(moveDir) > 0.0f) {
        moveDir = glm::normalize(moveDir);
        m_wantedVelocity.x = moveDir.x * currentSpeed;
        m_wantedVelocity.z = moveDir.z * currentSpeed;
    }
    else {
        m_wantedVelocity.x = 0.0f;
        m_wantedVelocity.z = 0.0f;
    }

    for (int i = 0; i <= 9; i++) {
        if (glfwGetKey(window, i + 48) == GLFW_PRESS) {
            m_currentBLock = std::clamp(i, 1, (int)BlockTable::getSize());
        }
    }
}

void Player::mouseCallBack(GLFWwindow* window)
{
    auto currentWorld = worldManager::getCurrentWorld();
    if (currentWorld) {
        auto currentDimension = currentWorld->getCurrentDimension();
        if (currentDimension) {
            bool leftMouse = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
            bool rightMouse = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
            
            if (leftMouse || rightMouse) {
                RaycastHit hit = raycastVoxels(currentDimension, Camera::getPos(), Camera::getFront(), 5.0f);
                if (hit.type == RaycastHit::HitType::Block) {
                    if (leftMouse) {
                        currentDimension->setBlock(hit.blockPos.x, hit.blockPos.y, hit.blockPos.z, 0);
                    }
                    if (rightMouse) {
                        auto playerBox = getCollisionMesh().globalBounds.offset(getPos());
                        collisionSystem::AABB blockBox{ glm::vec3(hit.adjacentBlockPos), glm::vec3(hit.adjacentBlockPos) + glm::vec3(1,1,1) };

                        bool Xcoll = !!collisionSystem::checkCollisionX(playerBox, blockBox);
                        bool Ycoll = !!collisionSystem::checkCollisionY(playerBox, blockBox);
                        bool Zcoll = !!collisionSystem::checkCollisionZ(playerBox, blockBox);
                        if (!(Xcoll && Ycoll && Zcoll)) {
                            currentDimension->setBlock(hit.adjacentBlockPos.x, hit.adjacentBlockPos.y, hit.adjacentBlockPos.z, m_currentBLock);
                        }
                    }
                }
            }
        }
    }
    
}

void Player::MoveTo(glm::vec3 dest) {
    m_transform.pos = dest;
    if (m_model) {
        m_model->MoveTo(dest);
    }
}

void Player::MoveOn(glm::vec3 delta) {
    m_transform.pos += delta;
    if (m_model) {
        m_model->MoveOn(delta);
    }
}

void Player::Rotate(glm::vec3 deltaRotation) {
    m_transform.rotation += deltaRotation;

    if (m_model) {
        m_model->Rotate(deltaRotation);
    }
}