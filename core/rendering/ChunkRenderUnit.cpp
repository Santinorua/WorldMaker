#include "ChunkRenderUnit.h"
#include "Vertex.h"

namespace WorldMaker
{
	const int ChunkRenderUnit::chunkWidth = 10; // Amount of pixels per chunk
	const int ChunkRenderUnit::chunkHeight = 10; // Amount of pixels per chunk

	ChunkRenderUnit::ChunkRenderUnit(std::vector<Vertex>& p_vertices)
	{
		vertices->pushBatchData(p_vertices);
		vertices->submitData();

		indices->pushBatchData(GetIndicesForChunk());
		indices->submitData();
	}

	void ChunkRenderUnit::ChangeVertices(std::vector<Vertex>& p_vertices)
	{
		vertices->flush();
		vertices->pushBatchData(p_vertices);
		vertices->submitData();

		indices->flush();
		indices->pushBatchData(GetIndicesForChunk());
		indices->submitData();
	}

	std::vector<unsigned int> ChunkRenderUnit::GetIndicesForChunk()
	{
		std::vector<unsigned int> indices;

		for (int y = 0; y < chunkHeight-1; y++)
		{
			for (int x = 0; x < chunkWidth-1; x++)
			{
				unsigned int bottomLeft = y * chunkWidth + x;
				unsigned int bottomRight = y * chunkWidth + (x + 1);
				unsigned int topLeft = (y+1) * chunkWidth + x;
				unsigned int topRight = (y+1) * chunkWidth + (x +1);

				indices.push_back(bottomLeft);
				indices.push_back(bottomRight);
				indices.push_back(topRight);

				indices.push_back(bottomLeft);
				indices.push_back(topRight);
				indices.push_back(topLeft);
			}
		}
		return indices;
	}
}
