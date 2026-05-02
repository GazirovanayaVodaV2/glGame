#pragma once

#include <functional>
#include <glad/glad.h>   
#include <GLFW/glfw3.h>

#include "Idrawable.hpp"

class glfwContext {
private:
	static GLFWwindow* m_window;

	static std::vector<std::function<void()>> m_inCycleEvents;
	static std::vector<Idrawable*> m_drawableObjects;

	glfwContext() = default;
	~glfwContext();

	
public:
	static void init();

	static void updateConfiguration();

	static GLFWwindow* getWindow();

	static void addCycleEvent(std::function<void()> event);
	static void addDrawTarget(Idrawable* object);

	static void mainGameCycle();
};