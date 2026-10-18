#version 420 core
out vec4 FragColor;

in vec2 TexCoords;

in vec3 world_pos;
in vec3 normal;

uniform sampler2D ourTexture; 

layout (std140, binding = 0) uniform GlobalMatrices {
    mat4 projection;
    mat4 view;
    vec4 cameraPos;
    int time;
};

struct pointLightData {
    vec3 pos;
    vec3 color;
    float radius; 
    float power;
};

layout (std140, binding = 2) uniform GlobalData {
    int activeLights;
    pointLightData lights[64];
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

    vec3 lightColor = vec3(0);
    for (int i = 0; i < activeLights; i++) {
        vec3 lightDir = lights[i].pos - world_pos;
        float dist = length(lightDir);

        float attenuation = lights[i].power / (dist * dist + 1.0);

        float window = max(0.0, 1.0 - (dist / lights[i].radius + 0.001));
        float pwr = attenuation * (window * window);
        lightColor += lights[i].color * pwr;
    }

    vec4 texColor = texture(ourTexture, TexCoords);

    vec3 finalDiffuse = ambientColor + lightColor;
    vec3 finalColor = texColor.rgb * finalDiffuse;
    finalColor = clamp(finalColor, 0.0, 1.0); 

    FragColor = vec4(finalColor, texColor.a);
}