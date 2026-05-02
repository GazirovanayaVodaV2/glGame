#include <iostream>
#include <string_view>

#include "gameSettings.hpp"
#include "glfwContext.hpp"
#include "timer.hpp"

GLFWwindow* glfwContext::m_window = nullptr;

std::vector<std::function<void()>> glfwContext::m_inCycleEvents;
std::vector<Idrawable*> glfwContext::m_drawableObjects;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    gameSettings::resolution = { width, height };
    gameSettings::ratio = (float)gameSettings::resolution.first / (float)gameSettings::resolution.second;
}

void glfwContext::init()
{
    loadSettings();

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        std::exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        std::exit(-1);
    }

    if (gameSettings::fullscreen)
        glfwSetWindowPos(m_window, 0, 0);

    

    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
    glfwSwapInterval(gameSettings::vsync);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        std::exit(-1);
    }
}

void glfwContext::updateConfiguration()
{
}

glfwContext::~glfwContext()
{
    glfwTerminate();
}

void glfwContext::mainGameCycle()
{
    timer logicTimer, renderTimer;
    float accumulator = 0.0f;
    while (!glfwWindowShouldClose(m_window)) {
        const float targetFpsTime = 1000.0f / gameSettings::maxFps;
        
        accumulator += logicTimer.getDeltaTicks();
        glfwPollEvents();

        if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(m_window, true);

        while (accumulator >= 1.0f) {
            for (auto& event : m_inCycleEvents) {
                event();
            }
            accumulator -= 1.0f;
        }


        if (gameSettings::vsync || (renderTimer.getDeltaMS(false) >= targetFpsTime)) {
            renderTimer.reset();
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            for (auto& object : m_drawableObjects) {
                object->draw();
            }

            glfwSwapBuffers(m_window);
        }
    }
}

GLFWwindow* glfwContext::getWindow()
{
    return m_window;
}

void glfwContext::addCycleEvent(std::function<void()> event)
{
    m_inCycleEvents.push_back(event);
}

void glfwContext::addDrawTarget(Idrawable* object)
{
    m_drawableObjects.push_back(object);
}
