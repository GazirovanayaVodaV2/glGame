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
#include <bitset>
#include <tuple>

#include "../compression/compression.hpp"
#include "../utils/minTypes/minTypes.hpp"

#include "Block.hpp"

namespace worldConstants {
	static constexpr auto WIDTH = minUint<16>();
	static constexpr auto LENGTH = minUint<16>();
	static constexpr auto HEIGHT = minUint<255>();
	static constexpr auto SURFACE = minUint<WIDTH * HEIGHT>();
	static constexpr auto CHUNK_VOLUME = minUint<WIDTH * LENGTH * HEIGHT>();

	constexpr float gravity = 0.08f;
}

using chunkPosT = std::pair<int, int>;
using inChunkX_t = minUint_t<worldConstants::WIDTH>;
using inChunkY_t = minUint_t<worldConstants::HEIGHT>;
using inChunkZ_t = minUint_t<worldConstants::LENGTH>; 
using inChunkPos_t = std::tuple<inChunkX_t, inChunkY_t, inChunkZ_t>;

struct inChunkPosHash {
	std::size_t operator()(const inChunkPos_t & pos) const noexcept {
		auto x = static_cast<uint64_t>(std::get<0>(pos));
		auto y = static_cast<uint64_t>(std::get<1>(pos));
		auto z = static_cast<uint64_t>(std::get<2>(pos));
		return static_cast<std::size_t>((x << 32) | (y << 16) | z);
	}
};

struct ChunkPosHash {
	std::size_t operator()(const chunkPosT& pos) const noexcept {
		return (static_cast<std::uint64_t>(pos.first) << 32) |
			static_cast<std::uint32_t>(pos.second);
	}
};

namespace worldConstants {
	[[nodiscard]] inline chunkPosT worldCoordToChunkCoord(float x, float z) noexcept
	{
		constexpr auto SHIFT_X = std::countr_zero(WIDTH);
		constexpr auto SHIFT_Z = std::countr_zero(LENGTH);

		const auto ix = static_cast<int>(std::floor(x));
		const auto iz = static_cast<int>(std::floor(z));

		return { ix >> SHIFT_X, iz >> SHIFT_Z };
	}

	static_assert(std::has_single_bit(static_cast<unsigned int>(WIDTH)), "WIDTH must be a power of 2!");
	static_assert(std::has_single_bit(static_cast<unsigned int>(LENGTH)), "LENGTH must be a power of 2!");
};

struct blockArray {
	std::vector<BlockId_t> m_array = std::vector<BlockId_t>(worldConstants::CHUNK_VOLUME);
	void setBlock(inChunkX_t x, inChunkY_t y, inChunkZ_t z, BlockId_t id);
};
struct blockMetaArray {
	std::vector<BlockMeta_t> m_array = std::vector<BlockMeta_t>(worldConstants::CHUNK_VOLUME);
	void setMeta(inChunkX_t x, inChunkY_t y, inChunkZ_t z, BlockMeta_t meta);
};

using heightMapArray = std::array<std::uint8_t, worldConstants::WIDTH * worldConstants::LENGTH>;

struct RawMeshData {
	std::vector<vertex> vertices;
	std::vector<unsigned int> indices;
};

struct ChunkRawData {
	std::array<RawMeshData, worldConstants::SURFACE> meshes;
};

struct ChunkMeshData {
	std::array<RawMeshData, worldConstants::SURFACE> meshes;
};  

class meshBuilder {
public:
	meshBuilder() = default;
	~meshBuilder() = default;

	std::unique_ptr<ChunkRawData> buildMesh(const blockArray& blocks, const blockMetaArray& blockMeta, const heightMapArray& heightMap);
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
	BlockId_t id;
	BlockMeta_t meta;
};

class chunk {
private:
	bool loaded = false;
	bool arraysLoaded = false;
	bool meshBuilderDirty = true;
	std::vector<std::unique_ptr<ISceneObject>> entities;

	std::shared_ptr<chunkBuffers> m_buffers;
	compression m_compressedChunk;
	compression m_compressedChunkMeta;
	std::unordered_map<inChunkPos_t, chunkChanges, inChunkPosHash> m_changes;
	std::vector<SubChunkModel> m_terrainModels;

	std::future<std::unique_ptr<ChunkRawData>> m_meshFuture;
	bool m_isBuildingMesh = false;

	std::filesystem::path* worldPath = nullptr;

	int ix, iy;
	
	bool compressed = false;

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

	void setBlock(inChunkX_t x, inChunkY_t y, inChunkZ_t z, BlockId_t id, BlockMeta_t meta = 0);

	static int getIndex(inChunkX_t x, inChunkY_t y, inChunkZ_t z) {
		return x + (z * worldConstants::WIDTH) + (y * worldConstants::WIDTH * worldConstants::LENGTH);
	}

	BlockId_t getBlock(inChunkX_t lx, inChunkY_t ly, inChunkZ_t lz);
	BlockMeta_t getBlockMeta(inChunkX_t lx, inChunkY_t ly, inChunkZ_t lz);

	chunkPosT getChunkPos() {
		return { ix, iy };
	}

	void loadFromFile();
	void safeToFile();

	std::vector<SubChunkModel>& getTerrainModel() {
		return m_terrainModels;
	}

	bool isLoaded() {
		return loaded;
	}

	void compress();
	void decompress();

	bool isMeshBuilded() {
		return !m_isBuildingMesh && !meshBuilderDirty;
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
	//std::vector<> m_loadedChunks;
	std::unordered_map<chunkPosT, std::unique_ptr<chunk>, ChunkPosHash> m_loadedChunks;
	glm::vec2 pastRenderPos{};
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

	BlockId_t getBlock(int x, int y, int z);
	BlockMeta_t getBlockMeta(int x, int y, int z);

	chunk* getChunk(int chunkX, int chunkZ)
	{
		auto it = m_loadedChunks.find({ chunkX, chunkZ });
		return (it != m_loadedChunks.end()) ? it->second.get() : nullptr;
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