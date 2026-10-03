#pragma once

#include "../utils/timer/timer.hpp"
#include "../glfwContext.hpp"
#include "../renderer/renderer.hpp"



class application {
public:
	using voidFunction = std::function<void()>;
private:
	static inline timer logicTimer{}, renderTimer{};
	static inline float accumulator{}, alpha{};

	static inline std::vector<voidFunction> m_inCycleEvents;
	static inline std::vector<voidFunction> m_inCycleEvents_Undeletable;

	static void preGameloopInit();
public:
	application() = default;
	~application() = default;
	static void init();
	static void run();

	static void addCycleEvent(voidFunction event, bool canDelete = true) {
		if (canDelete) {
			m_inCycleEvents.push_back(event);
		}
		else {
			m_inCycleEvents_Undeletable.push_back(event);
		}
	}
	static void deleteAllCycleEvents() {
		m_inCycleEvents.clear();
	}
};