#pragma once

#include <functional>
#include <openGLIncluder.hpp>
#include <glm/mat4x4.hpp>
#include "Idrawable.hpp"
#include "shader.hpp"
#include "collisionSystem.hpp"
#include <glm/ext/vector_float3.hpp>

constexpr int OPENGLVERSION_MAJOR = 3;
constexpr int OPENGLVERSION_MINOR = 3;
constexpr int OPENGLVERSION = OPENGLVERSION_MAJOR * 10 + OPENGLVERSION_MINOR;

using voidFunction = std::function<void()>;
using inputEvent = std::function<void(GLFWwindow*)>;

class glfwContext {
private:
	static inline GLFWwindow* m_window = nullptr;

	static inline std::vector<voidFunction> m_inCycleEvents;
	static inline std::vector<voidFunction> m_inCycleEvents_Undeletable;
	static inline std::vector<inputEvent> m_keyboardEvents;
	static inline std::vector<inputEvent> m_mouseEvents;
	static inline std::vector<Idrawable*> m_drawableObjects;

	static inline int openGLVersion = OPENGLVERSION;

	glfwContext() = default;
	~glfwContext();
public:
	static inline unsigned int currentShaderID{};
	static inline glm::mat4 projection = glm::perspective(glm::radians(80.0f), 16.0f / 9.0f, 0.1f, 100.0f);

	static void init();

	static void updateConfiguration();

	static GLFWwindow* getWindow();

	static void addCycleEvent(voidFunction event, bool canDelete = true);
	static void addDrawTarget(Idrawable* object);

	static void addKeyboardInputEvent(inputEvent event);
	static void addMouseInputEvent(inputEvent event);

	static void deleteAllRenderTargets();
	static void deleteAllGameEvents();

	static void mainGameCycle();
	static void useShader(shader& _shader);

	/*vibe coded*/
	template <typename T, typename Res, typename... Args>
	static auto bindMember(T* instance, Res(T::* method)(Args...)) {
		return [instance, method](Args... args) {
			return (instance->*method)(std::forward<Args>(args)...);
			};
	}
	template <typename T, typename Res, typename... Args>
	static auto bindMember(T* instance, Res(T::* method)(Args...) const) {
		return [instance, method](Args... args) {
			return (instance->*method)(std::forward<Args>(args)...);
			};
	}

	template <typename T, typename Method>
	static void addKeyboardInputEvent(T* instance, Method method) {
		addKeyboardInputEvent(bindMember(instance, method));
	}

	template <typename T, typename Method>
	static void addMouseInputEvent(T* instance, Method method) {
		addMouseInputEvent(bindMember(instance, method));
	}

	template <typename T, typename Method>
	static void addCycleEvent(T* instance, Method method, bool canDelete = true) {
		addCycleEvent(bindMember(instance, method), canDelete);
	}
};