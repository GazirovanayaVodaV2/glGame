#pragma once
#include <fstream>
#include <glad/glad.h>
#include <filesystem>
#include <unordered_map>

#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp> 
#include <glm/mat4x4.hpp> 

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

	void use();

	int getUniformLocation(const std::string& name);
	template <typename T> 
	void set(const std::string& name, T value);
private:
	unsigned int ID;
	std::unordered_map<std::string, int> m_uniformLocationCache;

	unsigned int compile(std::string code, types type);
	void checkErrors(uint32_t shader, types type);
};

template<typename T>
inline void shader::set(const std::string& name, T value)
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
