#include "basicModel.hpp"

#include "../../worldMap/camera.hpp"
#include "../../renderer/renderer.hpp"

void basicModel::draw(float alpha)
{
    auto model = m_transform.getInterpolated(m_lastState, alpha);
    renderer::useShader(m_shader);
    m_texture.bind();
    m_mesh.bind();

    m_shader.set<glm::mat4>("model", model);
    glDrawElements(GL_TRIANGLES, m_mesh.getIndiciesSize(), GL_UNSIGNED_INT, 0);
    m_mesh.unbind();
}
 