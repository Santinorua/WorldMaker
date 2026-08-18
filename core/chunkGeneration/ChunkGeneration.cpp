#include "ChunkGeneration.h"
#include "ChunkRenderUnit.h"
#include "CoreGenerator.h"
#include <glm/gtc/quaternion.hpp>

namespace WorldMaker {

namespace ChunkGeneration {

ChunkRenderUnit* GenerateChunk(WorldGenerator& generator, float x_offset, float z_offset)
{
	int gridWidth = ChunkRenderUnit::s_chunkSide;
    int gridDepth = ChunkRenderUnit::s_chunkSide;

    // std::vector<double> pixels;
    // int width = 10;
    // int height = 10;
    // for (int x = 0; x < width; x++)
    // {
    //     for (int y = 0; y < height; y++)
    //     {
    //         pixels.push_back(1);
    //         pixels.push_back(1);
    //         pixels.push_back(1);
    //         pixels.push_back(1);
    //     }
    // }
    std::vector<Vertex> chunkVertices;
    chunkVertices.reserve(ChunkRenderUnit::s_chunkSide * ChunkRenderUnit::s_chunkSide);
    double tallestPoint = 0;
    double lowestPoint = 0;

    ChunkModels chunkModels;
    bool treeTest = false;
    for (int z = z_offset * ChunkRenderUnit::s_chunkSide - (z_offset != 0) * z_offset; z < ChunkRenderUnit::s_chunkSide * (z_offset + 1) - (z_offset != 0) * z_offset; ++z)
    {
        for (int x = x_offset * ChunkRenderUnit::s_chunkSide - (x_offset != 0) * x_offset; x < ChunkRenderUnit::s_chunkSide * (x_offset + 1) - (x_offset != 0) * x_offset; ++x)
        {

        	Vertex v;
            v = generator.getVertex(x, z);
            tallestPoint = std::max(tallestPoint, v.m_position.y);
            lowestPoint = std::min(lowestPoint, v.m_position.y);

			chunkVertices.push_back(v);
        }
    }

	for (int z = z_offset * ChunkRenderUnit::s_chunkSide - (z_offset != 0) * z_offset; z < ChunkRenderUnit::s_chunkSide * (z_offset + 1) - (z_offset != 0) * z_offset; ++z) {
		int localZ = z % ChunkRenderUnit::s_chunkSide;
		for (int x = x_offset * ChunkRenderUnit::s_chunkSide - (x_offset != 0) * x_offset; x < ChunkRenderUnit::s_chunkSide * (x_offset + 1) - (x_offset != 0) * x_offset; ++x) {
			int localX = x % ChunkRenderUnit::s_chunkSide;
			float hL = localX != 0 ? chunkVertices[localZ*gridWidth + localX-1].m_position.y : generator.getVertex(x-1, z).m_position.y;
			float hR = localX != ChunkRenderUnit::s_chunkSide - 1 ? chunkVertices[localZ*gridWidth + localX+1].m_position.y : generator.getVertex(x+1, z).m_position.y;
			float hU = localZ != 0 ? chunkVertices[(localZ-1)*gridWidth + localX].m_position.y : generator.getVertex(x, z-1).m_position.y;
			float hD = localZ != ChunkRenderUnit::s_chunkSide - 1 ? chunkVertices[(localZ+1)*gridWidth + localX].m_position.y : generator.getVertex(x, z+1).m_position.y;

			glm::vec3 normal;
			normal.x = static_cast<float>(hL - hR);
			normal.y = static_cast<float>(2.0 * 1);
			normal.z = static_cast<float>(hD - hU);

			normal = glm::normalize(normal);

			chunkVertices[(z % ChunkRenderUnit::s_chunkSide)*gridWidth + (x % ChunkRenderUnit::s_chunkSide)].m_normal = normal;
		}
	}
    return new ChunkRenderUnit(chunkVertices, tallestPoint, lowestPoint, chunkModels);
}
void RegenerateChunks(std::vector<ChunkRenderUnit*>& chunks, WorldGenerator& generator, int width, int height)
{
	chunks.clear();

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
		    chunks.push_back(GenerateChunk(generator, x, y));
		}
	}
}

}

}
