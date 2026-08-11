#include "ChunkGeneration.h"
#include "ChunkRenderUnit.h"
#include <glm/gtc/quaternion.hpp>

#define CLAMP0(x) ((x) * ((x) > 0))

namespace WorldMaker {

namespace ChunkGeneration {

ChunkRenderUnit* GenerateChunk(WorldGenerator& generator, int x_chunk, int z_chunk)
{
	int gridWidth = ChunkRenderUnit::s_chunkSide;
    int gridDepth = ChunkRenderUnit::s_chunkSide;

    std::vector<Vertex> chunkVertices;
    chunkVertices.reserve(ChunkRenderUnit::s_chunkSide * ChunkRenderUnit::s_chunkSide);
    double tallestPoint = 0;
    double lowestPoint = 0;

    ChunkModels chunkModels;
    bool treeTest = false;
    for (int z = z_chunk * ChunkRenderUnit::s_chunkSide - z_chunk; z < ChunkRenderUnit::s_chunkSide * (z_chunk + 1) - z_chunk; z++)
    {
    	int localX = 0;
        for (int x = x_chunk * ChunkRenderUnit::s_chunkSide - x_chunk; x < ChunkRenderUnit::s_chunkSide * (x_chunk + 1) - x_chunk; x++)
        {

        	Vertex v;
            v = generator.getVertex(x, z);
            if (!treeTest)
            {
                glm::quat rot = glm::identity<glm::quat>();
                chunkModels.addInstance("core/rendering/assets/models/tree.glb", v.m_position, rot, {0.1, 0.1f, 0.1f});
                treeTest = true;
            }
            tallestPoint = std::max(tallestPoint, v.m_position.y);
            lowestPoint = std::min(lowestPoint, v.m_position.y);

   //          v.m_color = {0.5,0.5,0.5,1};
   //
			// if (x % ChunkRenderUnit::s_chunkSide == 0 || x % ChunkRenderUnit::s_chunkSide == ChunkRenderUnit::s_chunkSide - 1) {
			// 	v.m_color.x = 1;
			// }
   //
			// if (z % ChunkRenderUnit::s_chunkSide == 0 || z % ChunkRenderUnit::s_chunkSide == ChunkRenderUnit::s_chunkSide - 1) {
			// 	v.m_color.y = 1;
			// }
			chunkVertices.push_back(v);
			localX++;
        }
    }
	int localZ = 0;
	for (int z = z_chunk * ChunkRenderUnit::s_chunkSide - z_chunk; z < ChunkRenderUnit::s_chunkSide * (z_chunk + 1) - z_chunk; z++) {
		// int localZ = (z + z_chunk) % ChunkRenderUnit::s_chunkSide;
		int localX = 0;
		for (int  x = x_chunk * ChunkRenderUnit::s_chunkSide - x_chunk; x < ChunkRenderUnit::s_chunkSide * (x_chunk + 1) - x_chunk; x++) {
			// int localX = (x + x_chunk) % ChunkRenderUnit::s_chunkSide;
			float hL = localX != 0 ? chunkVertices[localZ*gridWidth + localX-1].m_position.y : generator.getVertex(x-1, z).m_position.y;
			float hR = localX != ChunkRenderUnit::s_chunkSide - 1 ? chunkVertices[localZ*gridWidth + localX+1].m_position.y : generator.getVertex(x+1, z).m_position.y;
			float hU = localZ != 0 ? chunkVertices[(localZ-1)*gridWidth + localX].m_position.y : generator.getVertex(x, z-1).m_position.y;
			float hD = localZ != ChunkRenderUnit::s_chunkSide - 1 ? chunkVertices[(localZ+1)*gridWidth + localX].m_position.y : generator.getVertex(x, z+1).m_position.y;

			glm::vec3 normal;
			normal.x = static_cast<float>(hL - hR);
			normal.y = static_cast<float>(2.0 * 1);
			normal.z = static_cast<float>(hD - hU);

			// glm::vec3 R = localX != ChunkRenderUnit::s_chunkSide - 1 ? chunkVertices[localZ*gridWidth + localX+1].m_position : generator.getVertex(x+1, z).m_position;
			// glm::vec3 L = localX != 0 ? chunkVertices[localZ*gridWidth + localX-1].m_position : generator.getVertex(x-1, z).m_position;
			// glm::vec3 U = localZ != 0 ? chunkVertices[(localZ-1)*gridWidth + localX].m_position : generator.getVertex(x, z-1).m_position;
			// glm::vec3 D = localZ != ChunkRenderUnit::s_chunkSide - 1 ? chunkVertices[(localZ+1)*gridWidth + localX].m_position : generator.getVertex(x, z+1).m_position;
			//
			//
			// glm::vec3 tangentX = R - L;
			// glm::vec3 tangentY = D - U;
			//
			// glm::vec3 normal = ;



			normal = glm::normalize(normal);

			chunkVertices[localZ *gridWidth + localX].m_normal = normal;
			localX++;
		}
		localZ++;
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

void RegenerateChunks(ChunkArray& chunks, WorldGenerator& generator, int render_distance, glm::vec3 camera_pos)
{
	static glm::ivec2 last_pos = {INT_MAX, INT_MAX};
	glm::ivec2 pos = GetChunkPos(camera_pos);

	// TODO: Find if there's a better way
	if (pos == last_pos) {
		return;
	}
	last_pos = pos;

	for (int i = 0; i < chunks.size(); i++) {
		auto& chunk = chunks[i];

		auto ck_pos = chunk.first;
		if (glm::abs(pos.x - ck_pos.x) > render_distance || glm::abs(pos.y - ck_pos.y) > render_distance) {
			//printf("Deleting (%d, %d)\n", ck_pos.x, ck_pos.y);
			delete chunk.second;
			chunks.erase(std::next(chunks.begin(), i));
			i--;
		}
	}

	glm::ivec2 x_generation_range = GetGenerationRange(pos.x, render_distance);
	glm::ivec2 y_generation_range = GetGenerationRange(pos.y, render_distance);

	for (int x = x_generation_range.x; x < x_generation_range.y; x++) {
		for (int y = y_generation_range.x; y < y_generation_range.y; y++) {
			auto found = std::find_if(chunks.begin(), chunks.end(), [x, y](std::pair<glm::ivec2, ChunkRenderUnit*>& ck) {return ck.first == glm::ivec2{x, y};} );
			if (found != chunks.end()) continue;
			chunks.push_back({{x, y}, GenerateChunk(generator, x, y)});
		}
	}
}

}

}
