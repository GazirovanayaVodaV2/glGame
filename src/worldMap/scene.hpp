#pragma once

#include <array>
#include <vector>
#include <memory>
#include <utility>
#include <bitset>
#include "ISceneObject.hpp"
#include "Block.hpp"
#include "glm/vec2.hpp"

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

	constexpr float gravity = 0.98f;

	inline std::pair<int, int> worldCoordToChunkCoord(float x, float z)
	{
		auto _x = static_cast<int>(std::floor(x));
		auto _z = static_cast<int>(std::floor(z));

		auto ix = _x >> std::countr_zero(static_cast<unsigned int>(worldConstants::WIDTH));
		auto iz = _z >> std::countr_zero(static_cast<unsigned int>(worldConstants::LENGTH));

		return { ix, iz };
	}

	static_assert(std::has_single_bit(static_cast<unsigned int>(WIDTH)), "WIDTH must be a power of 2!");
	static_assert(std::has_single_bit(static_cast<unsigned int>(LENGTH)), "LENGTH must be a power of 2!");
	static_assert(std::has_single_bit(static_cast<unsigned int>(HEIGHT)), "HEIGHT must be a power of 2!");
}

struct blockArray {
	std::array<uint16_t, worldConstants::CHUNK_VOLUME> m_array;
	void setBlock(int x, int y, int z, int id);
};
struct blockMetaArray {
	std::array<uint8_t, worldConstants::CHUNK_VOLUME> m_array;
	void setMeta(int x, int y, int z, int meta);
};

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

struct chunkChanges {
	int x, y, z, id, meta;
};

class chunk {
private:
	bool loaded = false;
	bool arraysLoaded = false;
	bool meshBuilderDirty = true;
	std::vector<std::unique_ptr<ISceneObject>> entities;

	std::shared_ptr<chunkBuffers> m_buffers;
	std::vector<chunkChanges> m_changes;
	std::vector<SubChunkModel> m_terrainModels;

	std::future<ChunkRawData> m_meshFuture;
	bool m_isBuildingMesh = false;

	std::filesystem::path* worldPath = nullptr;

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
	chunk() = default;
	chunk(int xid, int yid, std::shared_ptr<chunkBuffers> buffers, std::filesystem::path* path);
	~chunk() = default;

	void update();
	void draw(float alpha);

	void setBlock(int x, int y, int z, int id, int meta = 0);

	static int getIndex(int x, int y, int z) {
		return x + (z * worldConstants::WIDTH) + (y * worldConstants::WIDTH * worldConstants::LENGTH);
	}

	int getBlock(int lx, int ly, int lz);
	int getBlockMeta(int lx, int ly, int lz);

	std::pair<int, int> getChunkPos() {
		return { ix, iy };
	}

	void loadFromFile();
	void safeToFile();

	std::vector<SubChunkModel>& getTerrainModel() {
		return m_terrainModels;
	}
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
	std::filesystem::path* worldPath = nullptr;

	void generateChunkPrep(int xid, int yid, int seed);
	virtual std::shared_ptr<chunkBuffers> generateChunk(int xid, int yid) {return std::make_shared<chunkBuffers>();};
	
public:
	dimensionBase() = default;
	dimensionBase(std::filesystem::path* path) : worldPath(path) {}
	~dimensionBase() = default;

	void loadChunksFromPos(glm::vec3 pos, int renderDistance, int seed);
	void update();
	void draw(float alpha);
	void setBlock(int x, int y, int z, int id, int meta = 0);

	int getBlock(int x, int y, int z);
	int getBlockMeta(int x, int y, int z);

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

	void saveAllLoadedChunks();
};

class overworld : public dimensionBase {
public:
	using dimensionBase::dimensionBase;
	std::shared_ptr<chunkBuffers> generateChunk(int xid, int yid) override;
};

class world : public Idrawable {
private:
	int m_seed = 0;
	std::string m_worldName;
	worldRules m_rules;
	std::array<std::unique_ptr<dimensionBase>, 3> m_dimensions;
	dimensionBase* currentDimension = nullptr;
	std::filesystem::path m_path;
	std::vector<ISceneObject*> m_globalObjects;
	std::unique_ptr<basicModel> skybox = std::make_unique<basicModel>("skybox", "skybox", "skyboxShader");
public:
	world();
	~world();

	world(std::string name, int seed, worldRules rules);

	void changeDimension(worldDimension dimension);
	dimensionBase* getCurrentDimension() {
		return currentDimension;
	}

	void loadChunksFromPos(glm::vec3 pos, int renderDistance);
	void draw(float alpha);
	void update();

	void addGlobalObject(ISceneObject* obj) { m_globalObjects.push_back(obj); }

	void SaveStateForInterpolation() override {}
};

class worldManager {
private:
	static inline std::vector<std::unique_ptr<world>> m_worlds;
	static inline world* currentWorld = nullptr;
public:
	worldManager();
	~worldManager() = default;

	static std::vector<std::unique_ptr<world>>& getList() {
		return m_worlds;
	}

	template <typename... Args>
	static inline void createWorld(Args&&... args) {
		m_worlds.emplace_back(std::make_unique<world>(std::forward<Args>(args)...));
	}

	static void loadWorld(int id);
	static world* getCurrentWorld() {
		return currentWorld;
	}
};