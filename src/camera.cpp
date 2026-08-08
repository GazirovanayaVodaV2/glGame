#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "camera.hpp"
#include "gameSettings.hpp"
#include "glfwContext.hpp"
#include "timer.hpp"

glm::vec3 Camera::pos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 Camera::front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 Camera::up = glm::vec3(0.0f, 1.0f, 0.0f);
float Camera::yaw = -90.0f;
float Camera::pitch = 0.0f;
float Camera::lastMouseX = 0.0f, Camera::lastMouseY = 0.0f;
glm::mat4 Camera::view = glm::mat4(1.0f);

glm::vec3 Camera::pastPos = Camera::pos;

void Camera::updateVectors()
{
	glm::vec3 direction{};
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	front = glm::normalize(direction);
	view = glm::lookAt(pos, pos + front, up);
}

void Camera::Rotate(float xoffset, float yoffset)
{
	xoffset *= gameSettings::mouseSens;
	yoffset *= gameSettings::mouseSens;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)  pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;
	updateVectors();
}

void Camera::MoveTo(glm::vec3 dest)
{
	pos = dest;
	updateVectors();
}

void Camera::MoveOn(glm::vec3 delta)
{
	pos += front * delta.z;
	pos += glm::normalize(glm::cross(front, up)) * delta.x;

	updateVectors();
}

void Camera::MoveByKeyBoard(MoveDirection dir)
{
	if (!isConnected) {
		float velocity = 1.0f;

		if (dir == MoveDirection::Forward)
			pos += front * velocity;
		if (dir == MoveDirection::Backward)
			pos -= front * velocity;
		if (dir == MoveDirection::Left)
			pos -= glm::normalize(glm::cross(front, up)) * velocity;
		if (dir == MoveDirection::Right)
			pos += glm::normalize(glm::cross(front, up)) * velocity;

		updateVectors();
	}
}

void Camera::mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	float xoffset = xpos - lastMouseX;
	float yoffset = lastMouseY - ypos;

	lastMouseX = xpos;
	lastMouseY = ypos;
	Camera::Rotate(xoffset, yoffset);
}

void Camera::keyCallback()
{
	auto window = glfwContext::getWindow();
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		MoveByKeyBoard(MoveDirection::Forward);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		MoveByKeyBoard(MoveDirection::Backward);

	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		MoveByKeyBoard(MoveDirection::Left);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		MoveByKeyBoard(MoveDirection::Right);
	}
}

void Camera::updateInterpolatedMatrix(float alpha)
{
	glm::vec3 interPos = glm::mix(pastPos, pos, alpha);

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	glm::vec3 interFront = glm::normalize(direction);
	view = glm::lookAt(interPos, interPos + interFront, glm::vec3(0, 1, 0));
}
