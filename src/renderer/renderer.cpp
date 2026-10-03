#include "renderer.hpp"

#include "../gameSettings/gameSettings.hpp"
#include "../global.hpp"
#include "../glfwContext.hpp"
#include "../worldMap/camera.hpp"

renderer::~renderer() {
	deleteAllRenderTargets();

    delete fbo[0];
    delete fbo[1];
    delete mainFBO;
    delete ppbuf;
}

void renderer::init() {
    renderer::projection = glm::perspective(glm::radians(gameSettings::fov), gameSettings::ratio, 0.1f, 1000.0f);

    UniformBuffer<globalUniforms_t>::init();
    UniformBuffer<globalUniforms_blackHolesData_t>::init(1);

    fbo[0] = new FrameBuffer();
    fbo[1] = new FrameBuffer();
    mainFBO = new FrameBuffer();
    ppbuf = new postProcessorBuffer();
}

void renderer::addDrawTarget(Idrawable* object) {
    m_drawableObjects.push_back(object);
}

void renderer::deleteAllRenderTargets() {
    m_drawableObjects.clear();
}

void renderer::useShader(shader& _shader) {
    auto id = _shader.getID();
    if (currentShaderID != id) {
        glUseProgram(id);
        currentShaderID = id;
    }
}

void renderer::drawAll(float alpha) {
    globalUniforms.view = Camera::getView();
    globalUniforms.cameraPos = glm::vec4(Camera::getPos(), 1.0f);
    globalUniforms.projection = renderer::projection;

    UniformBuffer<globalUniforms_t>::update(globalUniforms);

    glBindFramebuffer(GL_FRAMEBUFFER, mainFBO->getFBO());
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& object : m_drawableObjects) {
        object->tryDraw(alpha);
	}

    auto size = glfwContext::getScreenSize();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    ppbuf->draw(mainFBO->getFrameTextureID(), fbo);

    glfwSwapBuffers(glfwContext::getWindow());
}