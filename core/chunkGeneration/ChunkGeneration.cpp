#include "ChunkGeneration.h"
#include "ChunkRenderUnit.h"
#include "CoreGenerator.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/geometric.hpp>

#define LOD_LEVELS 5
#define CLAMP0(x) ((x) * ((x) > 0))

#define GENERATION_COORD(v) (((float)(v) / ChunkRenderUnit::s_chunkRes) * ChunkRenderUnit::s_chunkScale)

#define CHECK_LOD_DIST(ck_dist, render_distance, thresh) (((float)(ck_dist).x / (render_distance) > (thresh)) || ((float)(ck_dist).y / (render_distance) > (thresh)))


namespace WorldMaker {

	namespace ChunkGeneration {


void GenerateVertices(WorldGenerator& generator, int x_chunk, int z_chunk, int lod, ChunkModels *chunkModels, std::vector<Vertex> &chunkVertices, double &tallest_point, double &lowest_point)
{
	chunkVertices.clear();

	int side = (ChunkRenderUnit::s_chunkRes >> lod) + 1; // Resolution change
	int gridWidth = side; // other names because they were used
	int gridDepth = side;

	chunkVertices.reserve(side * side);

	// TODO: Add scale for chunks

	for (int z = z_chunk * side - z_chunk; z < side * (z_chunk + 1) - z_chunk; z++)
	{
		int localX = 0;
		for (int x = x_chunk * side - x_chunk; x < side * (x_chunk + 1) - x_chunk; x++)
		{

			generatorVertex v;
			v = generator.getVertex(GENERATION_COORD(x<<lod), GENERATION_COORD(z<<lod));

			tallest_point = glm::max(v.m_position.y, tallest_point);
			lowest_point = glm::max(v.m_position.y, lowest_point);

			if (v.m_featureId != 0 && chunkModels != nullptr)
			{
				chunkModels->addInstance(FeatureManager::m_features[v.m_featureId - 1].modelPath, {v.m_position.x, v.m_position.y + FeatureManager::m_features[v.m_featureId - 1].y_offset, v.m_position.z},
						FeatureManager::m_features[v.m_featureId - 1].rotation, FeatureManager::m_features[v.m_featureId - 1].scale);
			}
			chunkVertices.push_back(v.toGraphicalVertex());
			localX++;
		}
	}

	int localZ = 0;
	for (int z = z_chunk * side - z_chunk; z < side * (z_chunk + 1) - z_chunk; z++) {
		// int localZ = (z + z_chunk) % side;
		int localX = 0;
		for (int  x = x_chunk * side - x_chunk; x < side * (x_chunk + 1) - x_chunk; x++) {
			// int localX = (x + x_chunk) % side;
			float hL = localX != 0 ? chunkVertices[localZ*gridWidth + localX-1].m_position.y : generator.getVertex(GENERATION_COORD((x-1)<<lod), GENERATION_COORD(z<<lod)).m_position.y;
			float hR = localX != side - 1 ? chunkVertices[localZ*gridWidth + localX+1].m_position.y : generator.getVertex(GENERATION_COORD((x+1)<<lod), GENERATION_COORD(z<<lod)).m_position.y;
			float hU = localZ != 0 ? chunkVertices[(localZ-1)*gridWidth + localX].m_position.y : generator.getVertex(GENERATION_COORD(x<<lod), GENERATION_COORD((z-1)<<lod)).m_position.y;
			float hD = localZ != side - 1 ? chunkVertices[(localZ+1)*gridWidth + localX].m_position.y : generator.getVertex(GENERATION_COORD(x<<lod), GENERATION_COORD((z+1)<<lod)).m_position.y;

			glm::vec3 normal;
			normal.x = static_cast<float>(hL - hR);
			normal.y = static_cast<float>(2.0 * 1);
			normal.z = static_cast<float>(hD - hU);

			// glm::vec3 R = localX != side - 1 ? chunkVertices[localZ*gridWidth + localX+1].m_position : generator.getVertex(x+1, z).m_position;
			// glm::vec3 L = localX != 0 ? chunkVertices[localZ*gridWidth + localX-1].m_position : generator.getVertex(x-1, z).m_position;
			// glm::vec3 U = localZ != 0 ? chunkVertices[(localZ-1)*gridWidth + localX].m_position : generator.getVertex(x, z-1).m_position;
			// glm::vec3 D = localZ != side - 1 ? chunkVertices[(localZ+1)*gridWidth + localX].m_position : generator.getVertex(x, z+1).m_position;
			//
			//
			// glm::vec3 tangentX = R - L;
			// glm::vec3 tangentY = D - U;
			//
			// glm::vec3 normal = ;

			normal = glm::normalize(normal);

			chunkVertices[localZ * gridWidth + localX].m_normal = normal;
			localX++;
		}
		localZ++;
	}
}

ChunkRenderUnit* GenerateChunk(WorldGenerator& generator, int x_chunk, int z_chunk, int lod)
{
	std::vector<Vertex> chunkVertices;

	ChunkModels chunkModels;
	double tallestPoint = 0;
	double lowestPoint = 0;
	int side = (ChunkRenderUnit::s_chunkRes >> lod) + (lod != 0); // Resolution change
	int gridWidth = side; // other names because they were used
	int gridDepth = side;


	chunkVertices.reserve(side * side);

	// TODO: Add scale for chunks

	GenerateVertices(generator, x_chunk, z_chunk, lod, &chunkModels, chunkVertices, tallestPoint, lowestPoint);

	return new ChunkRenderUnit(chunkVertices, tallestPoint, lowestPoint, chunkModels, lod);
}

glm::ivec2 GetChunkPos(glm::vec3 pos) {
	int x = (pos.x - 1) / ChunkRenderUnit::s_chunkScale;
	int y = (pos.z - 1) / ChunkRenderUnit::s_chunkScale;
	if (pos.x < 0) x--;
	if (pos.z < 0) y--;

	return {x, y};
}

glm::ivec2 GetGenerationRange(int pos, int render_distance) {
	return {CLAMP0(pos - render_distance), pos + render_distance};
}

void RegenerateChunks(ChunkArray& chunks, WorldGenerator& generator, int render_distance, glm::vec3 camera_pos, bool force)
{
	static glm::ivec2 last_pos = {INT_MAX, INT_MAX};
	glm::ivec2 pos = GetChunkPos(camera_pos);

	if (pos == last_pos && !force) {
		return;
	}
	last_pos = pos;

	glm::ivec2 x_generation_range = GetGenerationRange(pos.x, render_distance);
	glm::ivec2 y_generation_range = GetGenerationRange(pos.y, render_distance);

	for (int x = x_generation_range.x; x < x_generation_range.y; x++) {
		for (int y = y_generation_range.x; y < y_generation_range.y; y++) {
			auto found = std::find_if(chunks.begin(), chunks.end(), [x, y](std::pair<glm::ivec2, ChunkRenderUnit*>& ck) {return ck.first == glm::ivec2{x, y};} );
			if (found != chunks.end()) continue;
			chunks.push_back({{x, y}, GenerateChunk(generator, x, y, ChunkRenderUnit::MAX_LOD)});
		}
	}

	for (int i = 0; i < chunks.size(); i++) {
		auto& chunk = chunks[i];

		auto ck_pos = chunk.first;
		auto ck_dist = glm::abs(pos - ck_pos);
		if (ck_dist.x > render_distance || ck_dist.y > render_distance) {
			//printf("Deleting (%d, %d)\n", ck_pos.x, ck_pos.y);
			delete chunk.second;
			chunks.erase(std::next(chunks.begin(), i));
			i--;
			continue;
		}
		double tallest, lowest;
		if (CHECK_LOD_DIST(ck_dist, render_distance, 0.80)) {
			if (!chunk.second->hasLOD(4)) {
				std::vector<Vertex> vertices;
				GenerateVertices(generator, ck_pos.x, ck_pos.y, 4, nullptr, vertices, tallest, lowest);
				chunk.second->uploadLOD(vertices, 4, tallest, lowest);
			}
			chunk.second->setLOD(4);
			continue;
		}

		if (CHECK_LOD_DIST(ck_dist, render_distance, 0.60)) {
			if (!chunk.second->hasLOD(3)) {
				std::vector<Vertex> vertices;
				GenerateVertices(generator, ck_pos.x, ck_pos.y, 3, nullptr, vertices, tallest, lowest);
				chunk.second->uploadLOD(vertices, 3, tallest, lowest);
			}
			chunk.second->setLOD(3);
			continue;
		}

		if (CHECK_LOD_DIST(ck_dist, render_distance, 0.45)) {
			if (!chunk.second->hasLOD(2)) {
				std::vector<Vertex> vertices;
				GenerateVertices(generator, ck_pos.x, ck_pos.y, 2, nullptr, vertices, tallest, lowest);
				chunk.second->uploadLOD(vertices, 2, tallest, lowest);
			}
			chunk.second->setLOD(2);
			continue;
		}

		if (pos == ck_pos && !chunk.second->hasLOD(1)) {
			printf("Generating lod 1\n");
		}

		if (!chunk.second->hasLOD(0)) {
			std::vector<Vertex> vertices;
			GenerateVertices(generator, ck_pos.x, ck_pos.y, 0, nullptr, vertices, tallest, lowest);
			chunk.second->uploadLOD(vertices, 0, tallest, lowest);
		}
		chunk.second->setLOD(0);
	}
}

}

}
