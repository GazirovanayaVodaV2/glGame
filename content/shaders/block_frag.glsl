#version 420 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D ourTexture; 

layout (std140, binding = 0) uniform GlobalMatrices {
    mat4 projection;
    mat4 view;
    vec4 cameraPos;
    int time;
};

vec3 getAmbientColor(float t) {
    vec3 cNight = vec3(0.247, 0.169, 0.588);
    vec3 cDusk  = vec3(1.000, 0.682, 0.051);
    vec3 cDay   = vec3(1.000, 1.000, 1.000);

    if (t < 0.08) {
        return cNight;
    } else if (t < 0.12) {
        return mix(cNight, cDusk, (t - 0.08) / 0.04);
    } else if (t < 0.16) {
        return mix(cDusk, cDay, (t - 0.12) / 0.04);
    } else if (t < 0.84) {
        return cDay;
    } else if (t < 0.88) {
        return mix(cDay, cDusk, (t - 0.84) / 0.04);
    } else if (t < 0.92) {
        return mix(cDusk, cNight, (t - 0.88) / 0.04);
    } else {
        return cNight;
    }
}

void main() {
    float t = fract(float(time) / 24000.0);

    vec3 ambientColor = getAmbientColor(t);
    vec4 texColor = texture(ourTexture, TexCoords);
    FragColor = vec4(texColor.rgb * ambientColor, texColor.a);
}