#version 420 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;

struct BlackHoleInfo {
    vec3 pos;
    float radius;
};

layout (std140, binding = 1) uniform GlobalData {
    BlackHoleInfo holes[4];
};


void main() {
    vec3 col = texture(screenTexture, TexCoords).rgb;

    float maxDarkness = 0.0;

    for (int i = 0; i < 4; i++) {
        if (holes[i].radius <= 0.0) continue;
        float distanceToCamera = length(holes[i].pos);

        float effectZone = holes[i].radius * 5.0; 

        if (distanceToCamera < effectZone) {
            float darknessFactor = 1.0 - (distanceToCamera / effectZone);
            
            maxDarkness = max(maxDarkness, darknessFactor);
        }
    }

    vec3 finalColor = col * (1.0 - maxDarkness);

    FragColor = vec4(finalColor, 1.0f);
}