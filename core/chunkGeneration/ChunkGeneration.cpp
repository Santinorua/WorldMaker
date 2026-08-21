#include "ChunkGeneration.h"
#include "ChunkRenderUnit.h"
#include "CoreGenerator.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/geometric.hpp>

#define LOD_LEVELS 5

#define CLAMP0(x) ((x) * ((x) > 0))

namespace WorldMaker {

	namespace ChunkGeneration {

		ChunkRenderUnit* GenerateChunk(WorldGenerator& generator, int x_chunk, int z_chunk)
		{
			std::vector<std::vector<Vertex>> chunkVertices{LOD_LEVELS};

			ChunkModels chunkModels;
			double tallestPoint = 0;
			double lowestPoint = 0;
			for (int i = 0; i < LOD_LEVELS; i++) {
				int side = (ChunkRenderUnit::s_chunkSide >> i) + (i != 0); // Resolution change
				int gridWidth = side; // other names because they were used
				int gridDepth = side;


				chunkVertices[i].reserve(side * side);

				// TODO: Add scale for chunks

				for (int z = z_chunk * side - z_chunk; z < side * (z_chunk + 1) - z_chunk; z++)
				{
					int localX = 0;
					for (int x = x_chunk * side - x_chunk; x < side * (x_chunk + 1) - x_chunk; x++)
					{

						generatorVertex v;
						v = generator.getVertex(x<<i, z<<i);
						if (v.m_featureId != 0 && i == 0)
						{

							chunkModels.addInstance(FeatureManager::m_features[v.m_featureId - 1].modelPath, {v.m_position.x, v.m_position.y + FeatureManager::m_features[v.m_featureId - 1].y_offset, v.m_position.z},
									FeatureManager::m_features[v.m_featureId - 1].rotation, FeatureManager::m_features[v.m_featureId - 1].scale);
						}
						tallestPoint = std::max(tallestPoint, v.m_position.y);
						lowestPoint = std::min(lowestPoint, v.m_position.y);
						chunkVertices[i].push_back(v.toGraphicalVertex());
						localX++;
					}
				}

				int localZ = 0;
				for (int z = z_chunk * side - z_chunk; z < side * (z_chunk + 1) - z_chunk; z++) {
					// int localZ = (z + z_chunk) % side;
					int localX = 0;
					for (int  x = x_chunk * side - x_chunk; x < side * (x_chunk + 1) - x_chunk; x++) {
						// int localX = (x + x_chunk) % side;
						float hL = localX != 0 ? chunkVertices[i][localZ*gridWidth + localX-1].m_position.y : generator.getVertex((x-1)<<i, z<<i).m_position.y;
						float hR = localX != side - 1 ? chunkVertices[i][localZ*gridWidth + localX+1].m_position.y : generator.getVertex((x+1)<<i, z<<i).m_position.y;
						float hU = localZ != 0 ? chunkVertices[i][(localZ-1)*gridWidth + localX].m_position.y : generator.getVertex(x<<i, (z-1)<<i).m_position.y;
						float hD = localZ != side - 1 ? chunkVertices[i][(localZ+1)*gridWidth + localX].m_position.y : generator.getVertex(x<<i, (z+1)<<i).m_position.y;

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

						chunkVertices[i][localZ * gridWidth + localX].m_normal = normal;
						localX++;
					}
					localZ++;
				}
			}
    return new ChunkRenderUnit(chunkVertices, tallestPoint, lowestPoint, chunkModels);
}

glm::ivec2 GetChunkPos(glm::vec3 pos) {
	int x = (pos.x - 1) / (ChunkRenderUnit::s_chunkSide - 1);
	int y = (pos.z - 1) / (ChunkRenderUnit::s_chunkSide - 1);
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
			chunks.push_back({{x, y}, GenerateChunk(generator, x, y)});
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

		if ((float)ck_dist.x / render_distance > 0.80) {
			chunk.second->setLOD(4);
			continue;
		}

		if ((float)ck_dist.x / render_distance > 0.60) {
			chunk.second->setLOD(3);
			continue;
		}

		if ((float)ck_dist.x / render_distance > 0.45) {
			chunk.second->setLOD(2);
			continue;
		}
		chunk.second->setLOD(1);
	}
}

}

}
