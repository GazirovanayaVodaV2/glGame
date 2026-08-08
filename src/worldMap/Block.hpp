#pragma once
#include "ISceneObject.hpp"
#include "assetManager/models/basicModel.hpp"

#include <vector>
#include <assetManager/textures/texture.hpp>

enum class BlockMaterial {
    Air,
    Wood,
    Stone,
    Glass,
    Grass,
};

struct BlockStats {
    float durability, explosionResistance;
};

class Block : public ISceneObject {
private:
	glm::vec3 m_position;
    BlockMaterial m_material;
	basicModel& m_model;
	int state = 0;
    float hp = 100.0f;
    BlockStats m_stats;
public:
    Block(basicModel& model, BlockMaterial material, BlockStats stats) 
        : m_model(model), m_material(material), m_stats(stats) {}
    void update() override {
        m_model.update();
    } 
    void draw(float alpha) {
        m_model.tryDraw(alpha);
    }

    auto getState() {
        return state;
    }
    auto getHp() {
        return hp;
    }

	void MoveTo(glm::vec3 dest) override {
		m_position = dest;
		m_model.MoveTo(dest);
	}
	void MoveOn(glm::vec3 delta) override {
		m_position += delta;
		m_model.MoveOn(delta);
	}
	void Rotate(glm::vec3 deltaRotation) override {
		m_model.Rotate(deltaRotation);
	}

	texture& getTexture() {
		return m_model.getTexture();
	}

	std::string getTextureName() {
		return m_model.getTextureName();
	}
	std::string getMeshName() {
		return m_model.getMeshName();
	}
	std::string getShaderName() {
		return m_model.getShaderName();
	}
};

class BlockTable {
private:
	static inline std::vector<std::unique_ptr<Block>> m_blocks;

public:
	BlockTable() = default;
	~BlockTable() = default;
	template <typename... Args>
    static void add(Args&&... args) {
		m_blocks.push_back(std::make_unique<Block>(std::forward<Args>(args)...));
	}
	static size_t getSize() {
		return m_blocks.size();
	}
	static Block* getBlock(int id) {
		if (m_blocks.empty()) {
			throw std::runtime_error("BlockTable is empty!");
		}
		if (id <= 0) return nullptr;
		id--;
		if (id >= 0 && id < m_blocks.size()) {
			return m_blocks[id].get();
		}
		return nullptr;
	}

	static texture& getTexture(int id) {
		if (m_blocks.empty()) {
			throw std::runtime_error("BlockTable is empty!");
		}
		if (id <= 0) {
			throw std::runtime_error("Attempted to get texture for Air (id = 0) or negative ID!");
		}
		id--;
		if (id >= 0 && id < m_blocks.size()) {
			return m_blocks[id]->getTexture();
		}
		throw std::runtime_error("Invalid block ID");
	}

	static std::string getTextureName(int id) {
		if (m_blocks.empty()) {
			throw std::runtime_error("BlockTable is empty!");
		}
		if (id <= 0) {
			throw std::runtime_error("Attempted to get texture name for Air (id = 0) or negative ID!");
		}
		id--;		if (id >= 0 && id < m_blocks.size()) {
			return m_blocks[id]->getTextureName();
		}
		throw std::runtime_error("Invalid block ID");
	}
	static std::string  getMeshName(int id) {

		if (m_blocks.empty()) {
			throw std::runtime_error("BlockTable is empty!");
		}

		if (id <= 0) {
			throw std::runtime_error("Attempted to get mesh name for Air (id = 0) or negative ID!");
		}
		id--;
		if (id >= 0 && id < m_blocks.size()) {
			return m_blocks[id]->getMeshName();
		}
		throw std::runtime_error("Invalid block ID");
	}
	static std::string  getShaderName(int id) {
		if (m_blocks.empty()) {
			throw std::runtime_error("BlockTable is empty!");
		}
		if (id <= 0) {
			throw std::runtime_error("Attempted to get shader name for Air (id = 0) or negative ID!");
		}
		id--;
		if (id >= 0 && id < m_blocks.size()) {
			return m_blocks[id]->getShaderName();
		}
		throw std::runtime_error("Invalid block ID");
	}
};