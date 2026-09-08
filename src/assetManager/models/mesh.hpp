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

      
            other.VAO = 0;
            other.VBO = 0;
            other.EBO = 0;
        }
        return *this;
    }

	void bind();
	void unbind();
	size_t getIndiciesSize() {
		return m_indicies.size();
	}

	bool isloaded() {
		return !m_verticies.empty() && !m_indicies.empty();
	}

    const std::vector<vertex>& getVertices() const { return m_verticies; }
    const std::vector<unsigned int>& getIndices() const { return m_indicies; }
    auto getUV() {
        /*std::vector<glm::vec2> res;
        for (auto& vert : m_verticies) {
            res.emplace_back(vert.uv);
        }
        return res;*/
        return m_verticies | std::views::transform([](const vertex& v) -> const glm::vec2& {
            return v.uv;
            });
    }
};