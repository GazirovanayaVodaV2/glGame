#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <filesystem>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <ranges>

struct vertex {
	glm::vec3 pos, normal;
	glm::vec2 uv;
};

class mesh {
private:
	unsigned int VAO, VBO, EBO;
	
	std::vector<vertex> m_verticies;
	std::vector<unsigned int> m_indicies;
    std::vector<glm::vec2> uvs;
    size_t m_indicesCount = 0;

    bool m_loaded = false;

	void setup();
    void cleanup();
public:
	mesh() = default;
	mesh(std::filesystem::path path);
	mesh(std::vector<vertex> verticies, std::vector<unsigned int> indicies);

	mesh(const mesh&) = delete;
	mesh& operator=(const mesh&) = delete;

    ~mesh() { cleanup(); }


    mesh(mesh&& other) noexcept
        : VAO(other.VAO), VBO(other.VBO), EBO(other.EBO),
        m_verticies(std::move(other.m_verticies)),
        m_indicies(std::move(other.m_indicies))
    {
        m_indicesCount = std::exchange(other.m_indicesCount, 0);
        uvs = std::move(other.uvs);
        m_loaded = std::exchange(other.m_loaded, 0);

        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
    }

    mesh& operator=(mesh&& other) noexcept {
        if (this != &other) {
            cleanup(); 

            VAO = other.VAO;
            VBO = other.VBO;
            EBO = other.EBO;
            m_verticies = std::move(other.m_verticies);
            m_indicies = std::move(other.m_indicies);

            m_indicesCount = std::exchange(other.m_indicesCount, 0);
            uvs = std::move(other.uvs);
            m_loaded = std::exchange(other.m_loaded, 0);

            other.VAO = 0;
            other.VBO = 0; 
            other.EBO = 0;
        }
        return *this;
    }

	void bind();
	void unbind();
	size_t getIndiciesSize() {
		return m_indicesCount;
	}

	bool isloaded() {
		return m_loaded;
	}

    const std::vector<vertex>& getVertices() const { return m_verticies; }
    const std::vector<unsigned int>& getIndices() const { return m_indicies; }
    auto& getUV() {
        /*
        return m_verticies | std::views::transform([](const vertex& v) -> const glm::vec2& {
            return v.uv;
            });*/
        return uvs;
    }
};