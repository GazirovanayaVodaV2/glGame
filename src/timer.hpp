#pragma once

#include <chrono>

class timer {
private:
	std::chrono::steady_clock::time_point m_startTime, m_stopTime;
public:
	static constexpr int ticksPerSecond = 20;
	using NanoSeconds = std::chrono::duration<float, std::nano>;
	using MilliSeconds = std::chrono::duration<float, std::milli>;
	using Ticks = std::chrono::duration<float, std::ratio<1, ticksPerSecond>>;

	timer() { start(); };
	~timer() = default;

	template<typename T>
	float getDelta(bool _reset = true);

	float getDeltaNS(bool reset = true);
	float getDeltaMS(bool reset = true);
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
