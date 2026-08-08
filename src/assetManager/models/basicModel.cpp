#include "basicModel.hpp"
#include "glfwContext.hpp"
#include "camera.hpp"

void basicModel::draw(float alpha)
{
    auto model = m_transform.getInterpolated(m_lastState, alpha);
    glfwContext::useShader(m_shader);
    m_texture.bind();
    m_mesh.bind();

    m_shader.set<glm::mat4>("model", model);
    m_shader.set<glm::mat4>("view", Camera::getView());
    m_shader.set<glm::mat4>("projection", glfwContext::projection);
    glDrawElements(GL_TRIANGLES, m_mesh.getIndiciesSize(), GL_UNSIGNED_INT, 0);
    m_mesh.unbind();
}
