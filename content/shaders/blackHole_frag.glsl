#version 420 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D ourTexture;
uniform sampler2D u_screenTexture;
uniform vec2 u_resolution;
uniform float radius;

layout (std140, binding = 0) uniform GlobalMatrices {
    mat4 projection;
    mat4 view;
    vec4 cameraPos;
    int time;
};

const float PI = 3.14159265359;

void main() {

    vec2 screenUV = gl_FragCoord.xy / u_resolution;

    vec2 offset = TexCoords - vec2(0.5);
    float dist = length(offset);
    if (dist > 0.5) {
        discard;
    }
    float eventHorizon = 0.15 * 0.5; 
    float shadowRadius = eventHorizon * 2.6; 

    if (dist < shadowRadius) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }


    

    float strength = 1.3;
    float power = 2.0;
    float factor = pow(eventHorizon / dist, power);

    float fade = smoothstep(0.5, 0.4, dist);

    vec2 distortDir = normalize(offset);
    float distortion = strength * factor * fade;

    vec2 distortedScreenUV = screenUV - (distortDir * distortion);

    FragColor = texture(u_screenTexture, distortedScreenUV);
}