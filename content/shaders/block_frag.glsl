#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D ourTexture; 

void main() {
    FragColor = texture(ourTexture, TexCoords);
    
    // FragColor = vec4(TexCoords, 0.0, 1.0); 

    //FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}