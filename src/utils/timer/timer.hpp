#pragma once

#include <chrono>
#include <vector>
#include <numeric>

class timer {
private:
	std::chrono::steady_clock::time_point m_startTime, m_stopTime;
public:
	static constexpr int ticksPerSecond = 20;
	using NanoSeconds = std::chrono::duration<float, std::nano>;
	using MilliSeconds = std::chrono::duration<float, std::milli>;
	using Seconds = std::chrono::duration<float>;

	using Ticks = std::chrono::duration<float, std::ratio<1, ticksPerSecond>>;

	timer() { start(); };
	~timer() = default;

	template<typename T>
	float getDelta(bool _reset = true);

	float getDeltaNS(bool reset = true);
	float getDeltaMS(bool reset = true);
	float getDeltaS(bool reset = true);
	float getDeltaTicks(bool reset = true);

	void reset();
	void start();

	static auto getTime() {
		return std::chrono::steady_clock::now();
	}
};

template<typename T>
inline float timer::getDelta(bool _reset)
{
	m_stopTime = getTime();
	auto delta = std::chrono::duration_cast<T>(m_stopTime - m_startTime).count();

	if (_reset) {
		reset();
	}
	return delta;
}

struct minorTaskPosition {
	unsigned char minimumTicks;
	unsigned char ticksBeforeStart;
};

struct minorTask {
public:
	std::vector<minorTaskPosition> minorTaskPositions;

	minorTask();
	~minorTask() = default;

	const auto getTicksBeforeStart(timer& _timer) {
		const auto deltaTicks = _timer.getDeltaTicks(false);
		auto minTicksInList{std::numeric_limits<decltype(minorTaskPosition::minimumTicks)>::max()};
		decltype(minorTaskPosition::ticksBeforeStart) lastValue{1};
		for (const auto& _minorTask : minorTaskPositions) {
			if (minTicksInList < _minorTask.minimumTicks) {
				minTicksInList = _minorTask.minimumTicks;
				lastValue = _minorTask.ticksBeforeStart;
			}
		}
		return lastValue;
	};
};