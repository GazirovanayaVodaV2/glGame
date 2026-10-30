#pragma once

namespace utils {
	namespace system {
		using memory_t = unsigned long long;
		enum class memorySize {
			Bytes,
			KiloBytes,
			MegaBytes,
			GigaBytes,
		};
		memory_t getFreeMemory(memorySize mSize = memorySize::Bytes);
	}
}