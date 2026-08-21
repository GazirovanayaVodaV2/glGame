#pragma once

#include <functional>
#include <openGLIncluder.hpp>
#include <glm/mat4x4.hpp>
#include "Idrawable.hpp"
#include "shader.hpp"
#include "collisionSystem.hpp"
#include <glm/ext/vector_float3.hpp>

#define OPENGLVERSION_MAJOR 3
#define OPENGLVERSION_MINOR 3
constexpr int OPENGLVERSION = OPENGLVERSION_MAJOR * 10 + OPENGLVERSION_MINOR;

using voidFunction = std::function<void()>;
using inputEvent = std::function<void(GLFWwindow*)>;

class glfwContext {
private:
	static GLFWwindow* m_window;

	static std::vector<voidFunction> m_inCycleEvents;
	static std::vector<voidFunction> m_inCycleEvents_Undeletable;
	static std::vector<Idrawable*> m_drawableObjects;

	static int openGLVersion;

	glfwContext() = default;
	~glfwContext();
public:
	static unsigned int currentShaderID;
	static glm::mat4 projection;

	static void init();

	static void updateConfiguration();

	static GLFWwindow* getWindow();

	static void addCycleEvent(voidFunction event, bool canDelete = true);
	static void addDrawTarget(Idrawable* object);

	static void deleteAllRenderTargets();
	static void deleteAllGameEvents();

	static void mainGameCycle();
	static void useShader(shader& _shader);
};