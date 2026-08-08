#pragma once

#include <string>
#include <utility>

struct gameSettings {
	static std::string gameName, windowName;
	static std::pair<int, int> resolution;
	static int maxFps;
	static bool vsync, fullscreen, resizeableWindow;
	static float ratio, mouseSens, fov;
};

void loadSettings();