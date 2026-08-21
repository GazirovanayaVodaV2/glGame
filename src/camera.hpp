#pragma once

#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <openGLIncluder.hpp>
#include "Icontrollable.hpp"

class Camera{
private:
	static glm::mat4 view;

	static glm::vec3 front;
	static glm::vec3 up;

	static glm::vec3 pos;
	static glm::vec3 pastPos;
	static glm::vec3 offset;
	static float yaw; 
	static float pitch;

	static float lastMouseX, lastMouseY;
	static inline bool isConnected = false;
	static inline Icontrollable* connectedObject = nullptr;

	static void updateVectors();
public:
	static void Rotate(float xoffset, float yoffset);
	static void MoveTo(glm::vec3 dest);
	static void MoveOn(glm::vec3 delta);
	static void MoveByKeyBoard(MoveDirection dir);

	static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);
	static void keyCallback();

	static void updateInterpolatedMatrix(float alpha);
	static void SaveStateForInterpolation() { pastPos = pos; };

	static void connectToObject(Icontrollable* obj) {
		connectedObject = obj;
		isConnected = !(obj == nullptr);
	}

	static void update() {
		if (connectedObject) {
			MoveTo(connectedObject->getPos());
			updateVectors();
		}
	}

	static glm::mat4 getView() {
		return view;
	}

	static glm::vec3 getPos() {
		return pos;
	}
	static glm::vec3 getFront() {
		return front;
	}

	static void setOffset(glm::vec3 _offset) {
		offset = _offset;
	}
};