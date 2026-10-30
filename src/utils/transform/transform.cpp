#include "transform.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp> 

glm::mat4 Transform::getModel()
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
    model *= glm::toMat4(orientation);
    model = glm::scale(model, scale);

    return model;
}

glm::mat4 Transform::getInterpolated(Transform& lastState, float alpha)
{
    glm::vec3 interpPos = glm::mix(lastState.pos, pos, alpha);
    glm::vec3 interpScale = glm::mix(lastState.scale, scale, alpha);

    glm::quat interpRot = glm::slerp(lastState.orientation, orientation, alpha);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, interpPos);
    model = model * glm::toMat4(interpRot);

    if (interpScale != glm::vec3(1.0f)) {
        model = glm::scale(model, interpScale);
    }

    return model;
}

void Transform::rotate(glm::vec3 degrees)
{
    orientation = glm::quat(glm::radians(degrees));
}

glm::vec3 Transform::getRotation()
{
    return glm::degrees(glm::eulerAngles(orientation));
}
