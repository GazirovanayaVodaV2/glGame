#include "transform.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp> 

glm::mat4 Transform::getModel()
{
	auto model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, scale);

    return model;
}

glm::mat4 Transform::getInterpolated(Transform& lastState, float alpha)
{
    glm::vec3 interpPos = glm::mix(lastState.pos, pos, alpha);
    glm::vec3 interpScale = glm::mix(lastState.scale, scale, alpha);

    glm::quat interpRot = glm::slerp(glm::quat(lastState.rotation), glm::quat(rotation), alpha);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, interpPos);
    model = model * glm::toMat4(interpRot);

    model = glm::scale(model, interpScale);

    return model;
}
