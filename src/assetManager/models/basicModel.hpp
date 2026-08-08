#pragma once
#include <string>
#include "mesh.hpp"
#include "assetManager/textures/texture.hpp"
#include "assetManager/assetManager.hpp"
#include "shader.hpp"

#include "transform.hpp"

#include "worldMap/ISceneObject.hpp"

class basicModel : public ISceneObject {
private:
	texture& m_texture;
	mesh& m_mesh;
	shader& m_shader;

	Transform m_transform, m_lastState;
	std::string m_textureName, m_meshName, m_shaderName;
public:
	basicModel() = delete;
	basicModel(std::string textureName, std::string meshName, std::string shaderName) 
		:m_shader(mainAssetManager::get<shader>(shaderName)),
		m_texture(mainAssetManager::get<texture>(textureName)),
		m_mesh(mainAssetManager::get<mesh>(meshName)),
		m_textureName(textureName),
		m_meshName(meshName),
		m_shaderName(shaderName)
	{}

	basicModel(std::string textureName, mesh& mesh, std::string shaderName)
		:m_shader(mainAssetManager::get<shader>(shaderName)),
		m_texture(mainAssetManager::get<texture>(textureName)),
		m_mesh(mesh),
		m_textureName(textureName),
		m_meshName(std::string()),
		m_shaderName(shaderName)
	{}

	basicModel(const basicModel&) = delete;
	basicModel& operator=(const basicModel&) = delete;

	~basicModel() = default;

	void MoveTo(glm::vec3 dest) override {
		m_lastState.pos = m_transform.pos;
		m_transform.pos = dest;
	};
	void MoveOn(glm::vec3 delta) override {
		m_lastState.pos = m_transform.pos;
		m_transform.pos += delta;
	};
	void Rotate(glm::vec3 deltaRotation) override {
		m_lastState.rotation = m_transform.rotation;
		m_transform.rotation += deltaRotation;
	};
	void Scale(glm::vec3 s) {
		m_lastState.scale = m_transform.scale;
		m_transform.scale = s;
	}
	void draw(float alpha) override;

	void update() override {}

	texture& getTexture() {
		return m_texture;
	}

	std::string getTextureName() {
		return m_textureName;
	}
	std::string getMeshName() {
		return m_meshName;
	}
	std::string getShaderName() {
		return m_shaderName;
	}
};