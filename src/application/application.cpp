#include "application.hpp"

#include "../worldMap/camera.hpp"
#include "../global.hpp"
#include "../gameSettings/gameSettings.hpp"
#include "../assetManager/assetManager.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    gameSettings::resolution = { width, height };
    gameSettings::ratio = (float)gameSettings::resolution.first / (float)gameSettings::resolution.second;
    renderer::setProjection(
        glm::perspective(
            glm::radians(gameSettings::fov),
            gameSettings::ratio, 0.1f, 100.0f
            )
        );
}

void application::init()
{
	glfwContext::init();
	renderer::init();

    glfwSetFramebufferSizeCallback(glfwContext::getWindow(), framebuffer_size_callback);
}

void application::preGameloopInit()
{
    auto& ppshader = mainAssetManager::get<shader>("basicPostProcess");
    renderer::addPostProcessorShader(&ppshader);

    logger::Console::print<logger::Level::INFO>("pre game loop init success");
}

void application::run()
{
    preGameloopInit();
    while (!glfwWindowShouldClose(glfwContext::getWindow())) {
        const float targetFpsTime = 1000.0f / gameSettings::maxFps;

        accumulator += logicTimer.getDeltaTicks();
        glfwPollEvents();

        while (accumulator >= 1.0f) {
            Camera::SaveStateForInterpolation();
            for (auto& event : m_inCycleEvents) {
                event();
            }
            for (auto& event : m_inCycleEvents_Undeletable) {
                event();
            }
            glfwContext::updateEvents();
            accumulator -= 1.0f;
        }

        alpha = accumulator;
        Camera::update();
        Camera::updateInterpolatedMatrix(alpha);


        if (gameSettings::vsync || (renderTimer.getDeltaMS(false) >= targetFpsTime)) {
			renderer::drawAll(alpha);
            renderTimer.reset();
        }
    }
}
