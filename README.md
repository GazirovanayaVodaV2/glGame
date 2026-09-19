Проект для практики opengl, а так же создание собственного клона Minecraft.

Используется Opengl 3.3 (С целью совместимости со старыми устройствами), [glfw](https://github.com/glfw/glfw), [glm](https://github.com/g-truc/glm), [stb](https://github.com/nothings/stb), [assimp](https://github.com/assimp/assimp) и [Nlohmann json](https://github.com/nlohmann/json)

![alt text](image.png)

Есть несколько вещей которые я планирую переделать:

1. Mesh builder не такой эффективный как хотелось бы.
2. Коллизии, игрок при определённых условиях проходит сквозь блоки.
3. Обработка кликов мыши.
4. Сжатие чанков (Текущая реализация без сжатия потребляет ~400 МБ озу при прогрузке радиусом 8 чанков)
5. Оптимизировать размер сохранённых карт

Сборка:
Через Visual Studio
или
В Developer Powershell запустить non_vs_build.cmd

Зависимости должны установится автоматически
