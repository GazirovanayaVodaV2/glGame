#include "blackHole.hpp"
#include "glfwContext.hpp"

#include "../renderer/renderer.hpp"

blackHole::blackHole(glm::vec3 spawnPos)
{
    m_model = std::make_unique<basicModel>("stone", "quad", "blackHoleShader");
    m_model->Scale({ 0.5,0.5,0.5 });
    m_model->MoveTo(spawnPos);

    MoveTo(spawnPos);
}

void blackHole::draw(float alpha)
{
    auto size = glfwContext::getScreenSize();
    glBindTexture(GL_TEXTURE_2D, fbo.getFrameTextureID());
    glCopyTexSubImage2D(
        GL_TEXTURE_2D,
        0, 0, 0, 0, 0,
        static_cast<GLsizei>(size.x),
        static_cast<GLsizei>(size.y)
    );

    auto& bhshader = mainAssetManager::get<shader>("blackHoleShader");
    renderer::useShader(bhshader);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fbo.getFrameTextureID());

    bhshader.set<float>("radius", getRadius());
    bhshader.set<int>("u_screenTexture", 1);
    bhshader.set<glm::vec2>("u_resolution", glfwContext::getScreenSize());
    if (m_model) {
        m_model->draw(alpha);
    } 
    glActiveTexture(GL_TEXTURE0); 
} 

void blackHole::MoveTo(glm::vec3 dest) {
    m_transform.pos = dest;
    if (m_model) {
        m_model->MoveTo(dest);
    }
}

void blackHole::MoveOn(glm::vec3 delta) {
    m_transform.pos += delta;
    if (m_model) {
        m_model->MoveOn(delta);
    }
}

void blackHole::Rotate(glm::vec3 deltaRotation) {
    m_transform.rotation += deltaRotation;

    if (m_model) {
        m_model->Rotate(deltaRotation);
    }
}