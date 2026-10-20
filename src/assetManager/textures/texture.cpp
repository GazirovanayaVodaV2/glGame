#include <iostream>

#include "texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "../../utils/logger/logger.hpp"

#include "openGLIncluder.hpp"

#define RGBA(r,g,b,a) r,g,b,a

void texture::load()
{
	if (m_loaded) {
		cleanup();
	}

	logger::Console::print<logger::Level::INFO>("Loading texture: " + m_path.string());

	
	stbi_set_flip_vertically_on_load(true);
	unsigned char* img = stbi_load(m_path.string().c_str(), &w, &h, &channels, 0);

	bool failed = false;

	if (!img) {
		failed = true;
		logger::Console::print<logger::Level::ERROR>("Failure reason: " + std::string(stbi_failure_reason()));
		
		w = 2, h = 2;
		channels = 4;
		img = (unsigned char*)malloc(w * h * channels * sizeof(unsigned char));
		unsigned char pattern[] = {
			RGBA(0,0,0,255), RGBA(255,255,255,255),
			RGBA(255,255,255,255), RGBA(0,0,0,255)
		};
		std::memcpy(img, pattern, sizeof(pattern));
	}

	glGenTextures(1, &id);

	GLenum format{};
	if (channels == 1) format = GL_RED;
	else if (channels == 3) format = GL_RGB;
	else if (channels == 4) format = GL_RGBA;

	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, img);

	if (failed) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else {
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		if (m_isLinear) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
	}

	stbi_image_free(img);
}

void texture::cleanup()
{
	glDeleteTextures(1, &id);
	m_loaded = false;
}

texture::texture(std::filesystem::path path, bool isLinear) 
	: m_path(path), m_isLinear(isLinear)
{
	load();
}

texture::~texture() {
	cleanup();
}

void texture::bind() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, id);
}