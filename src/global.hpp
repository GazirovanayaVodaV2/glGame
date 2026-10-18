#pragma once
#include <glm/glm.hpp>

struct alignas(16) globalUniforms_t {
    glm::mat4 projection;
    glm::mat4 view;
    alignas(16) glm::vec4 cameraPos;
    int time;
    int _pad[3];
};

static_assert(sizeof(globalUniforms_t) == 160, "UBO size must be multiple of 16!");

struct alignas(16) BlackHoleInfo {
    glm::vec3 pos;
    float radius;
};

struct globalUniforms_blackHolesData_t {
    BlackHoleInfo holes[4];
};

static_assert(sizeof(globalUniforms_blackHolesData_t) % 16 == 0, "UBO size must be multiple of 16!");

struct alignas(16) pointLightData {
    glm::vec3 m_pos{};
    float pad0;
    glm::vec3 m_color{ 0, 0, 0 };
    float m_radius = 1.0f, m_power = 1.0f;
    float pad1[2];
};

struct globalUniforms_pointLightData_t {
    int activeLights;
    float pad[3];
    pointLightData lights[64];
};

inline globalUniforms_t globalUniforms{};
inline globalUniforms_blackHolesData_t globalUniforms_blackHolesData{};
inline globalUniforms_pointLightData_t globalUniforms_pointLightData{};
