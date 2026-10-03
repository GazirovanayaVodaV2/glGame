#pragma once

#include <functional>

#include <glm/mat4x4.hpp>
#include <glm/ext/vector_float3.hpp>

#include "openGLIncluder.hpp"
#include "Interfaces/Idrawable.hpp"
#include "shader/shader.hpp"
#include "collisionSystem/collisionSystem.hpp"

constexpr int OPENGLVERSION_MAJOR = 4;
constexpr int OPENGLVERSION_MINOR = 2;
constexpr int OPENGLVERSION = OPENGLVERSION_MAJOR * 10 + OPENGLVERSION_MINOR;

class glfwContext {
public:
	using inputEvent = std::function<void(GLFWwindow*)>;
private:
	static inline GLFWwindow* m_window = nullptr;
	
	static inline std::vector<inputEvent> m_keyboardEvents;
	static inline std::vector<inputEvent> m_mouseEvents;

	static inline int openGLVersion = OPENGLVERSION;

	glfwContext() = default;
	~glfwContext();
public:
	static void init();
	static void updateConfiguration();
	static GLFWwindow* getWindow();
	static void addKeyboardInputEvent(inputEvent event);
	static void addMouseInputEvent(inputEvent event);

	static glm::vec2 getScreenSize();

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

	static void updateEvents() {
		for (auto& event : m_keyboardEvents) {
			event(m_window);
		}
		for (auto& event : m_mouseEvents) {
			event(m_window);
		}
	}
};