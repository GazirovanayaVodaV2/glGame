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


inline globalUniforms_t globalUniforms{};
inline globalUniforms_blackHolesData_t globalUniforms_blackHolesData{};
