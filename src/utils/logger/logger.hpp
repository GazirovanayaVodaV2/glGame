#pragma once
#include <string_view>
#include <iostream>
#include <format>

namespace logger {
	enum class Level {
		DEBUG = 0b01,
		INFO = 0b10,
		WARNING = 0b100,
		ERROR = 0b1000
	};

	constexpr auto operator|(Level lhs, Level rhs) {
		return static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs);
	}
	constexpr auto operator|(unsigned int lhs, Level rhs) {
		return lhs | static_cast<unsigned int>(rhs);
	}

#ifdef _DEBUG
	constexpr auto LOG_MASK = logger::Level::ERROR | logger::Level::WARNING | logger::Level::INFO | logger::Level::DEBUG;
#else
	constexpr auto LOG_MASK = logger::Level::ERROR | logger::Level::WARNING;
#endif

	template<typename Derived, unsigned int Mask = LOG_MASK>
	class Ilogger {
	public:
		template<logger::Level L = logger::Level::DEBUG>
		static inline void print(std::string_view ms) {
			constexpr unsigned int current_level_bit = static_cast<unsigned int>(L);
			constexpr bool is_enabled = (Mask & current_level_bit) != 0;

			if constexpr (is_enabled) {
				if constexpr (L == logger::Level::DEBUG) {
					Derived::template print_impl(std::format("[DEBUG] {}", ms));
				}
				else if constexpr (L == logger::Level::INFO) {
					Derived::template print_impl(std::format("[INFO] {}", ms));
				}
				else if constexpr (L == logger::Level::WARNING) {
					Derived::template print_impl(std::format("[WARNING!] {}", ms));
				}
				else if constexpr (L == logger::Level::ERROR) {
					Derived::template print_impl(std::format("[ERROR!] {}", ms));
				}
			}
		};
	};

	template<unsigned int Mask = LOG_MASK>
	class Console_impl : public Ilogger<Console_impl<Mask>, Mask> {
	public:
		friend class Ilogger<Console_impl<Mask>, Mask>;
	private:
		static inline void print_impl(std::string_view ms) {
			std::cout << ms << std::endl;
		}
	};


	using Console = Console_impl<LOG_MASK>;
}


