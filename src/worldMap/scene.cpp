#include "scene.hpp"

#include "assetManager/assetManager.hpp"
#include <algorithm>
#include <vector>
#include "Block.hpp"
#include <assetManager/models/mesh.hpp>
#include <bit>
#include <future>

chunk::chunk(int xid, int yid, std::shared_ptr<chunkBuffers> buffers)
{
	initBuffers();
	this->ix = xid;
	this->iy = yid;
	//to do later
	m_buffers = buffers;
	loaded = true;
}

void chunk::update()
{
	if (loaded) {
		for (const auto& entity : entities) {
			entity->update();
		}

		if (meshBuilderDirty && !m_isBuildingMesh) {
			m_isBuildingMesh = true;

			m_meshFuture = std::async(std::launch::async, [blocks = *m_buffers->blocks,
				meta = *m_buffers->blockMeta,
				height = *m_buffers->heightMap]() {
				m_threadPoolSemaphore.acquire();
				struct SemaphoreGuard {
					std::counting_semaphore<16>& sem;
					~SemaphoreGuard() { sem.release(); }
				} guard{ m_threadPoolSemaphore };
				meshBuilder builder;
				return builder.buildMesh(blocks, meta, height);
				});
		}

		if (m_isBuildingMesh && m_meshFuture.valid()) {
			if (m_meshFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
				auto rawData = m_meshFuture.get();
				glm::vec3 chunkWorldPos = {
					static_cast<float>(ix * worldConstants::WIDTH),
					0.0f,
					static_cast<float>(iy * worldConstants::LENGTH)
				};

				std::vector<SubChunkModel> newTerrainModels;
				int blockId = 0;
				for (const auto& rawMesh : rawData.meshes) {
					if (rawMesh.vertices.empty() || rawMesh.indices.empty()) {
						blockId++;
						continue; // Skip empty meshes
					}
					SubChunkModel sub;
					sub.subMesh = std::make_unique<mesh>(std::move(rawMesh.vertices), std::move(rawMesh.indices));

					auto texName = BlockTable::getTextureName(blockId);
					sub.model = std::make_unique<basicModel>(texName, *sub.subMesh, "blockShader");
					sub.model->MoveTo(chunkWorldPos);
					newTerrainModels.push_back(std::move(sub));
					blockId++;
				}
				m_terrainModels = std::move(newTerrainModels);
				meshBuilderDirty = false;
				m_isBuildingMesh = false;
			}
		}
		for (auto& subModel : m_terrainModels) {
			if (subModel.model) {
				subModel.model->update();
			}
			
		}
	}
}

void chunk::draw(float alpha) {
	//test code, change to better solution
	glm::vec3 chunkWorldPos = {
		static_cast<float>(ix * worldConstants::WIDTH),
		0.0f,
		static_cast<float>(iy * worldConstants::LENGTH)
	};
	//glLineWidth(2.0f); 
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (auto& sub : m_terrainModels) {
		if (sub.model) {
			sub.model->MoveTo(chunkWorldPos);
			sub.model->draw(alpha);
		}
	}
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void chunk::setBlock(int x, int y, int z, int id)
{
	(*m_buffers->blocks)[getIndex(x, y, z)] = id;
	meshBuilderDirty = true;
}

int chunk::getBlock(int lx, int ly, int lz)
{
	if (!m_buffers->blocks) return 0;
	size_t index = getIndex(lx, ly, lz);
	return (*m_buffers->blocks)[index];
}

std::string chunk::saveChunk()
{
	return std::string();
}


void dimensionBase::loadChunksFromPos(glm::vec3 pos, int renderDistance, int seed)
{
	auto halfRenderDistance = renderDistance / 2;
	int centerChunkX = static_cast<int>(std::floor(pos.x / worldConstants::WIDTH));
	int centerChunkZ = static_cast<int>(std::floor(pos.z / worldConstants::LENGTH));

	int minX = centerChunkX - halfRenderDistance;
	int maxX = centerChunkX + halfRenderDistance;
	int minZ = centerChunkZ - halfRenderDistance;
	int maxZ = centerChunkZ + halfRenderDistance;

	std::erase_if(m_loadedChunks, [minX, maxX, minZ, maxZ](const std::unique_ptr<chunk>& c) {
		auto [chunkX, chunkZ] = c->getChunkPos();
		return chunkX < minX || chunkX > maxX || chunkZ < minZ || chunkZ > maxZ;
		});

	for (int i = minX; i < maxX; i++) {
		for (int j = minZ; j < maxZ; j++) {
			generateChunkPrep(i, j, seed);
		}
	}
}

void dimensionBase::update()
{
	for (const auto& c : m_loadedChunks) {
		c->update();
	}
}

void dimensionBase::draw(float alpha)
{
	for (const auto& c : m_loadedChunks) {
		c->draw(alpha);
	}
}

void dimensionBase::setBlock(int x, int y, int z, int id)
{
	if (y < 0 || y >= worldConstants::HEIGHT) {
		return; // Out of bounds
	}
	
	int gridX = x >> std::countr_zero(static_cast<unsigned int>(worldConstants::WIDTH));
	int gridZ = z >> std::countr_zero(static_cast<unsigned int>(worldConstants::LENGTH));
	int localX = x & (worldConstants::WIDTH - 1);
	int localZ = z & (worldConstants::LENGTH - 1);

	auto targetChunk = getChunk(gridX, gridZ);

	if (!targetChunk) {
		return;
	}

	targetChunk->setBlock(localX, y, localZ, id);
}

int dimensionBase::getBlock(int x, int y, int z)
{
	if (y < 0 || y >= worldConstants::HEIGHT) return 0;

	int gridX = x >> std::countr_zero(static_cast<unsigned int>(worldConstants::WIDTH));
	int gridZ = z >> std::countr_zero(static_cast<unsigned int>(worldConstants::LENGTH));

	chunk* targetChunk = getChunk(gridX, gridZ);
	if (!targetChunk) return 0;

	return targetChunk->getBlock(x & (worldConstants::WIDTH - 1), y, z & (worldConstants::LENGTH - 1));
}

void dimensionBase::generateChunkPrep(int xid, int yid, int seed)
{
	bool alreadyLoaded = std::any_of(m_loadedChunks.begin(), m_loadedChunks.end(), [xid, yid](const std::unique_ptr<chunk>& c) {
		auto [chunkX, chunkY] = c->getChunkPos();
		return chunkX == xid && chunkY == yid;
		});

	if (!alreadyLoaded) {
		m_loadedChunks.emplace_back(std::make_unique<chunk>(xid, yid, generateChunk(xid, yid)));
	}
}

world::world(std::string name, int seed, worldRules rules)
	: m_worldName(name), m_seed(seed), m_rules(rules) 
{
	// init dimensions to the world
	m_dimensions[0] = std::make_unique<overworld>();
}

void world::changeDimension(worldDimension dimension)
{
	size_t index = static_cast<size_t>(dimension);
	if (index < m_dimensions.size()) {
		currentDimension = m_dimensions[index].get();
	}
	else {
		throw std::runtime_error("Invalid dimension index");
	}
}

void world::loadChunksFromPos(glm::vec3 pos, int renderDistance)
{
	if (currentDimension) {
		currentDimension->loadChunksFromPos(pos, renderDistance, this->m_seed);
	}
}

void world::draw(float alpha)
{
	if (currentDimension) {
		currentDimension->draw(alpha);
	}
}

void world::update()
{
	if (currentDimension) {
		currentDimension->update();
	}
}

std::shared_ptr<chunkBuffers> overworld::generateChunk(int xid, int yid)
{
	auto newgen = std::make_shared<chunkBuffers>();

	newgen->blocks = std::make_unique<blockArray>();
	newgen->blockMeta = std::make_unique<blockMetaArray>();
	newgen->heightMap = std::make_unique<heightMapArray>();

	size_t halfSize = newgen->blocks->size() / 2;
	std::fill_n(newgen->blocks->begin(), halfSize, 1);
	std::fill(newgen->blocks->begin() + halfSize, newgen->blocks->end(), 2);

	std::fill(newgen->blockMeta->begin(), newgen->blockMeta->end(), 0);
	std::fill(newgen->heightMap->begin(), newgen->heightMap->end(), 255);
	return newgen;
}

void meshBuilder::takeSnapshot(const blockArray& blocks, const blockMetaArray& blockMeta, const heightMapArray& heightMap)
{
	for (int x = 0; x < worldConstants::WIDTH; x++) {
		for (int z = 0; z < worldConstants::LENGTH; z++) {
			int maxHeight = std::clamp(heightMap[x + z * worldConstants::WIDTH], 0, worldConstants::HEIGHT - 1);
			for (int y = maxHeight; y >= 0; y--) {
				auto ind = chunk::getIndex(x, y, z);
				if (blocks[ind] != 0) {
					m_snapshot[chunk::getIndex(x, y, z)] = true;
				}
			}
		}
	}
}

ChunkRawData meshBuilder::buildMesh(const blockArray& blocks, const blockMetaArray& blockMeta, const heightMapArray& heightMap)
{
	std::array<std::vector<vertex>, 256> verticesMap;
	std::array<std::vector<unsigned int>, 256> indicesMap;

	auto isSolid = [&blocks](int x, int y, int z) -> bool {
		if (x < 0 || x >= worldConstants::WIDTH ||
			y < 0 || y >= worldConstants::HEIGHT ||
			z < 0 || z >= worldConstants::LENGTH) {
			return false;
		}
		return blocks[chunk::getIndex(x, y, z)] != 0;
		};

	auto addFace = [](std::vector<vertex>& vertices, std::vector<unsigned int>& indices,
		const glm::vec3& p0, const glm::vec3& p1,
		const glm::vec3& p2, const glm::vec3& p3,
		const glm::vec3& normal)
		{
			unsigned int indexOffset = static_cast<unsigned int>(vertices.size());
			vertices.emplace_back(p0, normal, glm::vec2{ 0.0f, 0.0f });
			vertices.emplace_back(p1, normal, glm::vec2{ 1.0f, 0.0f });
			vertices.emplace_back(p2, normal, glm::vec2{ 1.0f, 1.0f });
			vertices.emplace_back(p3, normal, glm::vec2{ 0.0f, 1.0f });

			indices.push_back(indexOffset + 0);
			indices.push_back(indexOffset + 1);
			indices.push_back(indexOffset + 2);
			indices.push_back(indexOffset + 0);
			indices.push_back(indexOffset + 2);
			indices.push_back(indexOffset + 3);
		};

	for (int x = 0; x < worldConstants::WIDTH; ++x) {
		for (int z = 0; z < worldConstants::LENGTH; ++z) {
			int maxHeight = std::clamp(heightMap[x + z * worldConstants::WIDTH], 0, worldConstants::HEIGHT - 1);
			for (int y = 0; y <= maxHeight; ++y) {

				int blockId = blocks[chunk::getIndex(x, y, z)];
				if (blockId == 0) continue;

				auto& verts = verticesMap[blockId];
				auto& inds = indicesMap[blockId];

				if (verts.capacity() == 0) {
					verts.reserve(1000);
				}
				if (inds.capacity() == 0) {
					inds.reserve(1500);
				}

				glm::vec3 pos(x, y, z);

				if (!isSolid(x, y + 1, z))
					addFace(verts, inds, pos + glm::vec3(0, 1, 1), pos + glm::vec3(1, 1, 1), pos + glm::vec3(1, 1, 0), pos + glm::vec3(0, 1, 0), { 0.0f, 1.0f, 0.0f });
				if (!isSolid(x, y - 1, z))
					addFace(verts, inds, pos + glm::vec3(0, 0, 0), pos + glm::vec3(1, 0, 0), pos + glm::vec3(1, 0, 1), pos + glm::vec3(0, 0, 1), { 0.0f, -1.0f, 0.0f });
				if (!isSolid(x + 1, y, z))
					addFace(verts, inds, pos + glm::vec3(1, 0, 1), pos + glm::vec3(1, 0, 0), pos + glm::vec3(1, 1, 0), pos + glm::vec3(1, 1, 1), { 1.0f, 0.0f, 0.0f });
				if (!isSolid(x - 1, y, z))
					addFace(verts, inds, pos + glm::vec3(0, 0, 0), pos + glm::vec3(0, 0, 1), pos + glm::vec3(0, 1, 1), pos + glm::vec3(0, 1, 0), { -1.0f, 0.0f, 0.0f });
				if (!isSolid(x, y, z + 1))
					addFace(verts, inds, pos + glm::vec3(0, 0, 1), pos + glm::vec3(1, 0, 1), pos + glm::vec3(1, 1, 1), pos + glm::vec3(0, 1, 1), { 0.0f, 0.0f, 1.0f });
				if (!isSolid(x, y, z - 1))
					addFace(verts, inds, pos + glm::vec3(1, 0, 0), pos + glm::vec3(0, 0, 0), pos + glm::vec3(0, 1, 0), pos + glm::vec3(1, 1, 0), { 0.0f, 0.0f, -1.0f });
			}
		}
	}

	ChunkRawData result;
	for (int blockId = 0; blockId < verticesMap.size(); ++blockId) {
		if (!verticesMap[blockId].empty()) {
			result.meshes[blockId] = RawMeshData{
				std::move(verticesMap[blockId]),
				std::move(indicesMap[blockId])
			};
		}
	}
	return result;
}
