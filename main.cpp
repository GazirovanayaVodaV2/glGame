#include "glfwContext.hpp"
#include "shader.hpp"

//AI generated square for testing
class Quad : public Idrawable {
private:
    unsigned int VAO, VBO, EBO;
    shader quad_shader;
public:
    Quad() {
        float vertices[] = {
            // Координаты (X, Y, Z)    // Цвета (R, G, B)
             0.5f,  0.5f, 0.0f,       0.1f, 0.8f, 0.2f, // Топ право
             0.5f, -0.5f, 0.0f,       0.1f, 0.8f, 0.2f, // Низ право
            -0.5f, -0.5f, 0.0f,       0.1f, 0.8f, 0.2f, // Низ лево
            -0.5f,  0.5f, 0.0f,       0.1f, 0.8f, 0.2f  // Топ лево
        };

        unsigned int indices[] = { 0, 1, 3, 1, 2, 3 };

        // 1. Генерируем объекты
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        // 2. Привязываем VAO (все настройки ниже запишутся в него)
        glBindVertexArray(VAO);

        // 3. Загружаем данные вершин
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // 4. Загружаем индексы
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // 5. Настраиваем атрибуты (Stride теперь 6 * float)
        // Позиция (location = 0)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Цвет (location = 1)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // 6. Отвязываем всё
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        quad_shader = shader("shaders/quad.glsl", "shaders/quad_frag.glsl");
    }

    void draw() override {
        quad_shader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    ~Quad() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
};


int main() {
    

    glfwContext::init();

    Quad* quad = new Quad();
    glfwContext::addDrawTarget(quad);

    glfwContext::mainGameCycle();

    delete quad;

    return 0;
}