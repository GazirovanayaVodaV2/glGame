#include "player.hpp"
#include "timer.hpp"
#include <algorithm>

#include "../glfwContext.hpp"


void Player::handeCollision(glm::vec3 mtv)
{
    MoveOn(mtv);
    m_onGround = true;
}

Player::Player(glm::vec3 spawnPos)
{
    m_model = std::make_unique<basicModel>("steve", "steve", "blockShader");
    m_model->Scale({0.5,0.5,0.5});
    m_model->MoveTo(spawnPos);
    m_collisionMesh.push_back({ {-0.3f, 0.0f, -0.3f}, {0.3f, 1.8f, 0.3f} });
    glfwContext::addCycleEvent([this]() { keyCallback(); }, true); //TEMP
    Camera::connectToObject(this);
    Camera::setOffset({0.0, 1.8f, 0.0f});

    MoveTo(spawnPos);
}

void Player::draw(float alpha) {
    if (m_mode != gameMode::SPECTATOR && m_model) {
        m_model->draw(alpha); //Model goin crazy so i fix this later
    }
}

void Player::update() {
    /*
    
    Rotate player model
    
    */
    auto cameraFront = Camera::getFront();
    auto cameraFrontXZ = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
    //float targetYaw = glm::degrees(glm::atan(cameraFrontXZ.x, cameraFrontXZ.z));
    float targetYaw = (glm::atan(cameraFrontXZ.x, cameraFrontXZ.z));
    float deltaYaw = targetYaw - m_transform.rotation.y;
    Rotate(glm::vec3(0.0f, deltaYaw, 0.0f));


    
    if (m_model) {
        m_model->update();
    }
    if (!m_onGround) {
        m_velocity *= 0.9;
    }
    else {
        m_velocity *= 0.6;
    }
    if (!m_onGround) {
       // m_velocity += glm::vec3(0.0f, -worldConstants::gravity, 0.0f);
    }
    MoveOn(m_velocity);
 
    m_onGround = false;
}

void Player::moveByKeyBoard(MoveDirection dir)
{

    glm::vec3 direction{};
    float yawRad = m_transform.rotation.y;
    float pitchRad = m_transform.rotation.x;

    glm::vec3 front;
    front.x = std::sin(yawRad) * std::cos(pitchRad);
    front.y = std::sin(pitchRad);
    front.z = std::cos(yawRad) * std::cos(pitchRad);
    front = glm::normalize(front);

    const glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(front, worldUp));
    glm::vec3 moveFront = m_isFlying ? front : glm::normalize(glm::vec3(front.x, 0.0f, front.z));
    glm::vec3 moveDir{ 0.0f };

    if (dir == MoveDirection::Forward)
        moveDir += moveFront;
    if (dir == MoveDirection::Backward)
        moveDir -= moveFront;
    if (dir == MoveDirection::Left)
        moveDir -= right;
    if (dir == MoveDirection::Right)
        moveDir += right;

    if (glm::length(moveDir) > 0.0f) {
        moveDir = glm::normalize(moveDir);
    }
    float currentSpeed = m_speed * (m_isSprinting ? 1.5f : 1.0f);
    m_velocity.x = moveDir.x * currentSpeed;
    m_velocity.z = moveDir.z * currentSpeed;
}

//to do fix input
void Player::keyCallback()
{
    auto window = glfwContext::getWindow();
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        moveByKeyBoard(MoveDirection::Forward);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        moveByKeyBoard(MoveDirection::Backward);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        moveByKeyBoard(MoveDirection::Left);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        moveByKeyBoard(MoveDirection::Right);
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