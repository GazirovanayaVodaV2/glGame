#pragma once

#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <openGLIncluder.hpp>

class Camera{
private:
	static glm::mat4 view;

	static glm::vec3 front;
	static glm::vec3 up;

	static glm::vec3 pos;
	static glm::vec3 pastPos;
	static float yaw; 
	static float pitch;

	static float lastMouseX, lastMouseY;
	static inline bool isConnected = false;
	static void updateVectors();
public:
	enum class MoveDirection {
		Forward,
		Backward,
		Left,
		Right
	};

	static void Rotate(float xoffset, float yoffset);
	static void MoveTo(glm::vec3 dest);
	static void MoveOn(glm::vec3 delta);
	static void MoveByKeyBoard(MoveDirection dir);

	static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);
	static void keyCallback();

	static void updateInterpolatedMatrix(float alpha);
	static void SaveStateForInterpolation() { pastPos = pos; };

	static glm::mat4 getView() {
		return view;
	}

	static glm::vec3 getPos() {
		return pos;
	}
	static glm::vec3 getFront() {
		return front;
	}
};