#version 330 core
out vec4 FragColor;

in vec2 TexCoords; // Имя должно СТРОГО совпадать с out из вершинного

uniform sampler2D ourTexture; 

void main() {
    // Пока просто выводим текстуру, чтобы проверить, что всё работает
    FragColor = texture(ourTexture, TexCoords);
    
    // Если хочешь проверить, доходят ли UV координаты, раскомментируй это:
    // FragColor = vec4(TexCoords, 0.0, 1.0); 

    //FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}