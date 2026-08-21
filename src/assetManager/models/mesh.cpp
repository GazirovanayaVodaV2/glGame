#include "mesh.hpp"

#include <iostream>

#include <glad/glad.h>

void mesh::setup()
{
	if (m_verticies.empty() || m_indicies.empty()) {
		return;
	}
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_verticies.size() * sizeof(vertex), m_verticies.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indicies.size() * sizeof(unsigned int), m_indicies.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, vertex::normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, vertex::uv));

    glBindVertexArray(0);

    //m_verticies.clear();
    //m_verticies.shrink_to_fit();
    //m_indicies.clear();
    //m_indicies.shrink_to_fit();
}

void mesh::cleanup()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

mesh::mesh(std::filesystem::path path)
{
	Assimp::Importer importer;

	if (std::filesystem::path(path).extension() == ".obj") {
		const aiScene* scene = importer.ReadFile(path.string(),
			aiProcess_Triangulate | aiProcess_GenSmoothNormals);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cerr << "Assimp Error: " << importer.GetErrorString() << std::endl;
			return;
		}

		for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[i];
            unsigned int vertexOffset = static_cast<unsigned int>(m_verticies.size());

            for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
                vertex vertex;

                vertex.pos = glm::vec3(
                    mesh->mVertices[j].x,
                    mesh->mVertices[j].y,
                    mesh->mVertices[j].z
                );

                vertex.normal = glm::vec3(
                    mesh->mNormals[j].x,
                    mesh->mNormals[j].y,
                    mesh->mNormals[j].z
                );

                if (mesh->mTextureCoords[0]) {
                    vertex.uv = glm::vec2(
                        mesh->mTextureCoords[0][j].x,
                        mesh->mTextureCoords[0][j].y
                    );
                }
                else {
                    vertex.uv = glm::vec2(0.0f, 0.0f);
                }

                m_verticies.push_back(vertex);
            }
            for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
                aiFace face = mesh->mFaces[j];
                for (unsigned int k = 0; k < face.mNumIndices; k++) {
                    m_indicies.push_back(face.mIndices[k] + vertexOffset);
                }
            }
		}
        setup();
        return;
	}
}

mesh::mesh(std::vector<vertex> verticies, std::vector<unsigned int> indicies)
{
    m_verticies = verticies;
    m_indicies = indicies;
    setup();
}

void mesh::bind() {
    glBindVertexArray(VAO);
}

void mesh::unbind() {
    glBindVertexArray(0);
}