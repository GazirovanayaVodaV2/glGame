#include "memory.hpp"

#include <cmath>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#error "Unsupported OS (todo: add linux support)"
#endif

static inline utils::system::memory_t getFreeMemoryImpl()
{
#if defined(_WIN32) || defined(_WIN64)

	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	if (GlobalMemoryStatusEx(&status)) {
		return status.ullAvailPhys;
	}
	return 0;

#else
#error "Unsupported OS (todo: add linux support)"
#endif
}

utils::system::memory_t utils::system::getFreeMemory(utils::system::memorySize mSize)
{
	return getFreeMemoryImpl() / (int)pow(1024.0f, (float)mSize);
}
