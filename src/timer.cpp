#include "timer.hpp"

float timer::getDeltaNS(bool reset)
{
	return getDelta<NanoSeconds>(reset);
}

float timer::getDeltaMS(bool reset)
{
	return getDelta<MilliSeconds>(reset);
}

float timer::getDeltaS(bool reset)
{
	return getDelta<Seconds>(reset);
}

float timer::getDeltaTicks(bool reset)
{
	return getDelta<Ticks>(reset);
}

void timer::start()
{
	m_startTime = getTime();
}

void timer::reset() {
	start();
}
