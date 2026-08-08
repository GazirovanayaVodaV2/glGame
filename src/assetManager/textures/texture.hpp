#pragma once

#include <string>
#include <filesystem>

class texture {
private:
	int w = 0, h = 0, channels = 0;
	unsigned int id = 0;
public:
	texture() = default;
	texture(std::filesystem::path path, bool isLinear = true);
	~texture();

	unsigned char* get();
	int getW() { return w; }
	int getH() { return h; }
	int getChannels() { return channels; }
	int getID() { return id; }

	void bind();
};