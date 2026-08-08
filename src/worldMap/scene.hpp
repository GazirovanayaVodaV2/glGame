#pragma once

#include <array>
#include <vector>
#include <memory>
#include <utility>
#include <bitset>
#include "ISceneObject.hpp"
#include "Block.hpp"
#include "glm/vec2.hpp"

#include "../player/player.hpp"
#include <assetManager/models/mesh.hpp>
#include <unordered_map>
#include <cstdint>
#include <bit>
#include <future>
#include <semaphore>
#include <thread>
#include <algorithm>

namespace worldConstants {
	static constexpr int WIDTH = 16;
	static constexpr int LENGTH = 16;
	static constexpr int HEIGHT = 256;
	static constexpr int CHUNK_VOLUME = WIDTH * LENGTH * HEIGHT;

	static_assert(std::has_single_bit(static_cast<unsigned int>(WIDTH)), "WIDTH must be a power of 2!");
	static_assert(std::has_single_bit(static_cast<unsigned int>(LENGTH)), "LENGTH must be a power of 2!");
	static_assert(std::has_single_bit(static_cast<unsigned int>(HEIGHT)), "HEIGHT must be a power of 2!");
}

using blockArray = std::array<uint16_t, worldConstants::CHUNK_VOLUME>;
using blockMetaArray = std::array<uint8_t, worldConstants::CHUNK_VOLUME>;
using heightMapArray = std::array<int, worldConstants::WIDTH * worldConstants::LENGTH>;

struct RawMeshData {
	std::vector<vertex> vertices;
	std::vector<unsigned int> indices;
};

struct ChunkRawData {
	std::array<RawMeshData, 256> meshes;
};

struct ChunkMeshData {
	std::array<RawMeshData, 256> meshes;
};  

class meshBuilder {
private:
	std::bitset<worldConstants::CHUNK_VOLUME> m_snapshot;

	void takeSnapshot(const blockArray& blocks, const blockMetaArray& blockMeta, const heightMapArray& heightMap);
public:
	meshBuilder() = default;
	~meshBuilder() = default;

	ChunkRawData buildMesh(const blockArray& blocks, const blockMetaArray& blockMeta, const heightMapArray& heightMap);
};

struct chunkBuffers {
	//to prevent allocations in default constructor
	std::unique_ptr<blockArray> blocks;
	std::unique_ptr<blockMetaArray> blockMeta;
	std::unique_ptr<heightMapArray> heightMap;
};

struct SubChunkModel {
	std::unique_ptr<mesh> subMesh;
	std::unique_ptr<basicModel> model;
};

class chunk {
private:

	static inline uint32_t getSafeThreadCount() {
		unsigned int cores = std::thread::hardware_concurrency();
		if (cores == 0) return 2;
		return std::min(16u, std::max(1u, cores - 1));
	}

	bool loaded = false;
	bool arraysLoaded = false;
	bool meshBuilderDirty = true;
	std::vector<std::unique_ptr<ISceneObject>> entities;

	std::shared_ptr<chunkBuffers> m_buffers;
	std::vector<SubChunkModel> m_terrainModels;

	static inline uint32_t MAX_THREADS = getSafeThreadCount();
	static inline std::counting_semaphore<16> m_threadPoolSemaphore{ MAX_THREADS };
	std::future<ChunkRawData> m_meshFuture;
	bool m_isBuildingMesh = false;

	int ix, iy;

	void initBuffers() {
		if (!arraysLoaded) {
			if (!m_buffers) {
				m_buffers = std::make_shared<chunkBuffers>();
			}
			m_buffers->blocks = std::make_unique<blockArray>();
			m_buffers->heightMap = std::make_unique<heightMapArray>();
			m_buffers->blockMeta = std::make_unique<blockMetaArray>();
			arraysLoaded = true;
		}
	}
public:
	chunk() {
		MAX_THREADS = getSafeThreadCount();
	};
	chunk(int xid, int yid, std::shared_ptr<chunkBuffers> buffers);
	~chunk() = default;

	void update();
	void draw(float alpha);

	void setBlock(int x, int y, int z, int id);

	static int getIndex(int x, int y, int z) {
		return x + (z * worldConstants::WIDTH) + (y * worldConstants::WIDTH * worldConstants::LENGTH);
	}

	int getBlock(int lx, int ly, int lz);

	std::pair<int, int> getChunkPos() {
		return { ix, iy };
	}

	std::string saveChunk();
};

enum class worldType {
	Normal,
	Flat,
	Custom
};

enum class worldDifficulty {
	Peaceful,
	Easy,
	Normal,
	Hard
};

enum class worldDimension {
	Overworld,
	Nether,
	End
};

struct worldRules {
	bool allowCheats = false;
	worldDifficulty difficulty = worldDifficulty::Normal;
	worldType type = worldType::Normal;
	worldDimension dimension = worldDimension::Overworld;
};

class dimensionBase {
protected:
	std::vector<std::unique_ptr<chunk>> m_loadedChunks;
	glm::vec2 pastRenderPos;

	void generateChunkPrep(int xid, int yid, int seed);
	virtual std::shared_ptr<chunkBuffers> generateChunk(int xid, int yid) {return std::make_shared<chunkBuffers>();};
public:
	dimensionBase() = default;
	~dimensionBase() = default;

	void loadChunksFromPos(glm::vec3 pos, int renderDistance, int seed);
	void update();
	void draw(float alpha);
	void setBlock(int x, int y, int z, int id);

	int getBlock(int x, int y, int z);

	chunk* getChunk(int chunkX, int chunkZ)
	{
		for (auto& ch : m_loadedChunks) {
			auto chunkPos = ch->getChunkPos();
			if (ch && chunkPos.first == chunkX && chunkPos.second == chunkZ) {
				return ch.get();
			}
		}
		return nullptr;
	}
};

class overworld : public dimensionBase {
public:
	std::shared_ptr<chunkBuffers> generateChunk(int xid, int yid) override;
};

class world : public Idrawable {
private:
	int m_seed = 0;
	std::string m_worldName;
	worldRules m_rules;
	std::array<std::unique_ptr<dimensionBase>, 3> m_dimensions;
	dimensionBase* currentDimension = nullptr;
public:
	world() = default;
	~world() = default;

	world(std::string name, int seed, worldRules rules);

	void changeDimension(worldDimension dimension);
	dimensionBase* getCurrentDimension() {
		return currentDimension;
	}

	void loadChunksFromPos(glm::vec3 pos, int renderDistance);
	void draw(float alpha);
	void update();
};

class worldManager {

};