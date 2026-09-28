#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform float radius;

layout (std140, binding = 0) uniform GlobalMatrices {
    mat4 projection;
    mat4 view;
    vec4 cameraPos;
    int time;
};

void main() {
    TexCoords = aTexCoords;

    vec4 viewCenter = view * model * vec4(0.0, 0.0, 0.0, 1.0);

    vec2 posOffset = aPos.xy * radius;
    vec4 viewPos = viewCenter + vec4(posOffset, 0.0, 0.0);

    gl_Position = projection * viewPos;
}