#include <iostream>

#include "shader.hpp"

static std::string readShader(std::filesystem::path path) {
	std::ifstream code(path);
	if (!code.is_open()) {
		std::cerr << "Failed to open shader! Path: " << path << std::endl;
		return "";
	}

	std::stringstream ss;
	ss << code.rdbuf();

	return ss.str();
}

unsigned int shader::compile(std::string code, shader::types type)
{
	const char* code_cstr = code.c_str();
	unsigned int shader = glCreateShader((int)type);
	glShaderSource(shader, 1, &code_cstr, NULL);
	glCompileShader(shader);
	checkErrors(shader, (shader::types)type);

	return shader;
}

void shader::checkErrors(uint32_t shader, shader::types type)
{
	int success;
	char log[1024];

	if (type != shader::types::program) {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, log);
			std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << (int)type << "\n" << log << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else {
		glGetShaderiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, NULL, log);
			std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << (int)type << "\n" << log << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}

shader::shader(std::filesystem::path vertexShader, std::filesystem::path fragmentShader)
{
	std::string vertexCode = readShader(vertexShader);
	std::string fragmentCode = readShader(fragmentShader);
	auto vertex = compile(vertexCode, types::vertex);
	auto fragment = compile(fragmentCode, types::fragment);

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	checkErrors(ID, types::program);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}


int shader::getUniformLocation(const std::string& name)
{
	if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end()) {
		return m_uniformLocationCache[name];
	}
	int loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cerr << "Warning, uniform " << name << " not found!" << std::endl;
		return -1;
	}
	m_uniformLocationCache[name] = loc;
	return loc;
}
