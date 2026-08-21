#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D skyboxTex; // basicModel передает 2D-текстуру

void main()
{
    FragColor = texture(skyboxTex, TexCoords);
}