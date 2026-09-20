#pragma once
#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <filesystem>
#include <unordered_map>

#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp> 
#include <glm/mat4x4.hpp> 

#include <string_view>

class shader {
public:
	enum class types {
		vertex = GL_VERTEX_SHADER,
		fragment = GL_FRAGMENT_SHADER,
		program = 0
	};
	shader() = default;
	shader(std::filesystem::path vertexShader,
		std::filesystem::path fragmentShader);
	~shader();

	shader(const shader&) = delete;
	shader& operator=(const shader&) = delete;
	shader(shader&& other) noexcept : ID(other.ID) { other.ID = 0; }

	int getUniformLocation(std::string_view name);
	template <typename T> 
	void set(std::string_view name, T value);

	unsigned int getID() {
		return ID;
	}



private:
	unsigned int ID = 0;

	std::unordered_map<std::string, int> m_uniformLocationCache;

	unsigned int compile(std::string code, types type);
	void checkErrors(uint32_t shader, types type);
};

template<typename T>
inline void shader::set(std::string_view name, T value)
{
	int location = getUniformLocation(name);
	if (location == -1) return;

	if constexpr (std::is_same_v<T, bool>) {
		glUniform1i(location, (int)value);
	}
	else if constexpr (std::is_same_v<T, int>) {
		glUniform1i(location, value);
	}
	else if constexpr (std::is_same_v<T, float>) {
		glUniform1f(location, value);
	}
	else if constexpr (std::is_same_v<T, glm::vec3>) {
		glUniform3fv(location, 1, glm::value_ptr(value));
	}
	else if constexpr (std::is_same_v<T, glm::mat4>) {
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
	else {
		static_assert(!sizeof(T), "Not supported uniform type!");
	}
}

template <typename T>
class UniformBuffer {
public:
	UniformBuffer() = delete;
	~UniformBuffer() = delete;
	UniformBuffer(const UniformBuffer&) = delete;
	UniformBuffer& operator=(const UniformBuffer&) = delete;

	static void init(GLuint bindingPoint = 0)
	{
		m_bindingPoint = bindingPoint;
		glGenBuffers(1, &ubo);
		if (ubo == GL_INVALID_VALUE) {
			std::cerr << "Warning, failed to create ubo" << std::endl;
		}

		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(T), nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingPoint, ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	static void update(const T& globalData) {
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), &globalData);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	static void free() {
		if (ubo != 0) {
			glDeleteBuffers(1, &ubo);
			ubo = 0;
		}
	}

	static unsigned int getID() { return ubo; }
private:
	static inline GLuint ubo{};
	static inline GLuint m_bindingPoint{};
};