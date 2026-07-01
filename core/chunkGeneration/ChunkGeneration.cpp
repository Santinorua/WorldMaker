#include "ChunkGeneration.h"

namespace WorldMaker {

namespace ChunkGeneration {

ChunkRenderUnit* GenerateChunk(FractalNoise& fractal, float x_offset, float z_offset)
{
	int gridWidth = ChunkRenderUnit::s_chunkSide;
    int gridDepth = ChunkRenderUnit::s_chunkSide;

    // std::vector<Vertex> chunkVertices;
    // chunkVertices.reserve(gridWidth * gridDepth);

    // FractalNoise fractal(gridWidth, gridDepth, 5, 2, 1, 4, 2.0, 0.75);
    // for (int z = 0; z < gridDepth; ++z)
    // {
    //     for (int x = 0; x < gridWidth; ++x)
    //     {

    //         double posY = fractal.getNoise(x, z);

    //         double hL = fractal.getNoise(x - 1, z);
    //         double hR = fractal.getNoise(x + 1, z);
    //         double hD = fractal.getNoise(x, z - 1);
    //         double hU = fractal.getNoise(x, z + 1);

    //         glm::vec3 normal;
    //         normal.x = static_cast<float>(hL - hR);
    //         normal.y = static_cast<float>(2.0 * 1);
    //         normal.z = static_cast<float>(hD - hU);

    //         normal = glm::normalize(normal);

    //         Vertex v;
    //         v.m_color = {1,1,1,1};
    //         v.m_uv = { static_cast<float>(x) / 10.0f, static_cast<float>(z) / 10.0f };
    //         v.m_position = { static_cast<float>(x), static_cast<float>(posY), static_cast<float>(z) };
    //         v.m_normal = normal;

    //         chunkVertices.push_back(v);
    //     }
    // }

    // ChunkRenderUnit chunk{chunkVertices};

    // Renderer::PrepareToDrawChunk(chunk);

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
    chunkVertices.reserve(ChunkRenderUnit::s_chunkSide * ChunkRenderUnit::s_chunkSide);
    double tallestPoint = 0;
    double lowestPoint = 0;

    for (int z = z_offset * ChunkRenderUnit::s_chunkSide - (z_offset != 0) * z_offset; z < ChunkRenderUnit::s_chunkSide * (z_offset + 1) - (z_offset != 0) * z_offset; ++z)
    {
        for (int x = x_offset * ChunkRenderUnit::s_chunkSide - (x_offset != 0) * x_offset; x < ChunkRenderUnit::s_chunkSide * (x_offset + 1) - (x_offset != 0) * x_offset; ++x)
        {

            double posY = fractal.getNoise(x, z);
            tallestPoint = std::max(tallestPoint, posY);
            lowestPoint = std::min(lowestPoint, posY);

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


			if (x % ChunkRenderUnit::s_chunkSide == 0 || x % ChunkRenderUnit::s_chunkSide == ChunkRenderUnit::s_chunkSide - 1) {
				v.m_color.x = 1;
			}

			if (z % ChunkRenderUnit::s_chunkSide == 0 || z % ChunkRenderUnit::s_chunkSide == ChunkRenderUnit::s_chunkSide - 1) {
				v.m_color.y = 1;
			}

			chunkVertices.push_back(v);
        }
    }

    return new ChunkRenderUnit{chunkVertices, tallestPoint, lowestPoint};
}

void RegenerateChunks(std::vector<ChunkRenderUnit*>& chunks, FractalNoise& fractal, int width, int height)
{
	for (ChunkRenderUnit *ck : chunks) {
		delete ck;
	}
	chunks.clear();

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			chunks.push_back(GenerateChunk(fractal, x, y));
		}
	}
}

}

}
