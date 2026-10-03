#include <iostream>
#include <string_view>

#include "global.hpp"
#include "glfwContext.hpp"
#include "gameSettings/gameSettings.hpp"
#include "utils/timer/timer.hpp"
#include "worldMap/camera.hpp"
#include "assetManager/assetManager.hpp"
#include "utils/logger/logger.hpp"

void glfwContext::init()
{
    loadSettings();

    if (!glfwInit()) {
		logger::Console::print<logger::Level::ERROR>("Failed to initialize GLFW");
        std::exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGLVERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGLVERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    if (gameSettings::fullscreen) {
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        gameSettings::resolution = { mode->width, mode->height };
    }
    else {
        if (gameSettings::resizeableWindow) {
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        }
        else {
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        }
    }

    m_window = glfwCreateWindow(gameSettings::resolution.first,
        gameSettings::resolution.second, gameSettings::windowName.c_str(),
        NULL, NULL);
    if (!m_window) {
        logger::Console::print<logger::Level::ERROR>("Failed to create GLFW window");
        glfwTerminate();
        std::exit(-1);
    }

    if (gameSettings::fullscreen)
        glfwSetWindowPos(m_window, 0, 0);

    

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(gameSettings::vsync);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(m_window, Camera::mouseCallback);
    int initRes = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    if (!initRes) {
        logger::Console::print<logger::Level::ERROR>("Failed to initialize GLAD");
        std::exit(-1);
    }

    GLint major = 0, minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    openGLVersion = (major * 10) + minor;
    if (openGLVersion < OPENGLVERSION) {
        logger::Console::print<logger::Level::ERROR>("Shitty GPU");
        std::exit(-1);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    addKeyboardInputEvent(Camera::keyCallback);
    addKeyboardInputEvent([](GLFWwindow* window) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        });
}

void glfwContext::updateConfiguration()
{
}

glfwContext::~glfwContext()
{
    glfwTerminate();
	logger::Console::print<logger::Level::INFO>("GLFW terminated");
}


glm::vec2 glfwContext::getScreenSize()
{
    int x = 0, y = 0;
    glfwGetWindowSize(m_window, &x, &y);

    return glm::vec2(static_cast<float>(x), static_cast<float>(y));
}

GLFWwindow* glfwContext::getWindow()
{
    return m_window;
}

void glfwContext::addKeyboardInputEvent(inputEvent event)
{
    m_keyboardEvents.push_back(event);
}

void glfwContext::addMouseInputEvent(inputEvent event)
{
    m_mouseEvents.push_back(event);
}
