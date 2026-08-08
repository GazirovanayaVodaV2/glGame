#include <filesystem>
#include <fstream>
#include <vector>
#include <iostream>

#include "gameSettings.hpp"
#include "nlohmann/json.hpp"

static std::filesystem::path windowConfigPath = "configs/settings.json";

#define getField(JSON, NAME, TYPE) JSON.at(NAME).get<TYPE>()

std::string gameSettings::gameName = "Project",
gameSettings::windowName = "Window";
std::pair<int, int> gameSettings::resolution = {640, 480};
int gameSettings::maxFps = 120;
bool gameSettings::vsync = false,
gameSettings::fullscreen = false,
gameSettings::resizeableWindow = true;

float gameSettings::ratio = 16.0f / 9.0f, gameSettings::mouseSens = 0.1f, gameSettings::fov = 80.0f;

void loadSettings()
{
    using json = nlohmann::json;

    std::ifstream configJson(windowConfigPath);
    if (!configJson.is_open()) {
        std::cerr << "Failed to open config! Path: " << windowConfigPath << std::endl;
        std::exit(-1);
    }

    auto parsedConfig = json::parse(configJson);

    gameSettings::gameName = APP_NAME;
    gameSettings::windowName = getField(parsedConfig, "windowName", std::string);   
    gameSettings::vsync = getField(parsedConfig, "vsync", bool);
    gameSettings::maxFps = getField(parsedConfig, "fps", int);
    gameSettings::fullscreen = getField(parsedConfig, "fullScreen", bool);   
    gameSettings::resizeableWindow = getField(parsedConfig, "resizeable", bool);

    auto resVector = getField(parsedConfig, "resolution", std::vector<int>);
    gameSettings::resolution = { resVector[0], resVector[1]};

    gameSettings::ratio = (float)gameSettings::resolution.first / (float)gameSettings::resolution.second;
}
