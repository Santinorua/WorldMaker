#include "ChunkGeneration.h"

namespace WorldMaker {

namespace ChunkGeneration {

ChunkRenderUnit* GenerateChunk(FractalNoise& fractal, float x_offset, float z_offset)
{
	int gridWidth = ChunkRenderUnit::chunkWidth;
    int gridDepth = ChunkRenderUnit::chunkHeight;

    std::vector<double> pixels;
    int width = 10;
    int height = 10;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            pixels.push_back(1);
            pixels.push_back(1);
            pixels.push_back(1);
            pixels.push_back(1);
        }
    }
    std::vector<Vertex> chunkVertices;
    chunkVertices.reserve(ChunkRenderUnit::chunkWidth * ChunkRenderUnit::chunkHeight);

    for (int z = z_offset * ChunkRenderUnit::chunkHeight - (z_offset != 0) * z_offset; z < ChunkRenderUnit::chunkHeight * (z_offset + 1) - (z_offset != 0) * z_offset; ++z)
    {
        for (int x = x_offset * ChunkRenderUnit::chunkWidth - (x_offset != 0) * x_offset; x < ChunkRenderUnit::chunkWidth * (x_offset + 1) - (x_offset != 0) * x_offset; ++x)
        {

            double posY = fractal.getNoise(x, z);

            double hL = fractal.getNoise(x - 1, z);
            double hR = fractal.getNoise(x + 1, z);
            double hD = fractal.getNoise(x, z - 1);
            double hU = fractal.getNoise(x, z + 1);

			glm::vec3 normal;
			normal.x = static_cast<float>(hL - hR);
			normal.y = static_cast<float>(2.0 * 1);
			normal.z = static_cast<float>(hD - hU);

			normal = glm::normalize(normal);

            Vertex v;
            v.m_color = {0.5,0.5,0.5,1};
            v.m_uv = { static_cast<float>(x) / 10.0f, static_cast<float>(z) / 10.0f };
			v.m_position = { static_cast<float>(x), static_cast<float>(posY), static_cast<float>(z)};
			v.m_normal = normal;


			if (x % ChunkRenderUnit::chunkWidth == 0 || x % ChunkRenderUnit::chunkWidth == ChunkRenderUnit::chunkWidth - 1) {
				v.m_color.x = 1;
			}

			if (z % ChunkRenderUnit::chunkHeight == 0 || z % ChunkRenderUnit::chunkHeight == ChunkRenderUnit::chunkHeight - 1) {
				v.m_color.y = 1;
			}

			chunkVertices.push_back(v);
        }
    }

    return new ChunkRenderUnit{chunkVertices};
}

void RegenerateChunks(std::vector<ChunkRenderUnit*>& chunks, FractalNoise& fractal, int render_distance, glm::vec3 pos)
{
	for (ChunkRenderUnit *ck : chunks) {
		delete ck;
	}
	chunks.clear();

	for (int x = 0; x < render_distance; x++) {
		for (int y = 0; y < render_distance; y++) {
			chunks.push_back(GenerateChunk(fractal, x, y));
		}
	}
}

}

}
