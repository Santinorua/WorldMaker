#include "ChunkRenderUnit.h"

namespace WorldMaker
{
	const int ChunkRenderUnit::chunkWidth = 10; // Amount of pixels per chunk
	const int ChunkRenderUnit::chunkHeight = 10; // Amount of pixels per chunk

	ChunkRenderUnit::ChunkRenderUnit(std::vector<Vertex>& p_vertices)
	{
		vertices->addBatchData(p_vertices);
		vertices->submitData();

		indices->addBatchData(GetIndicesForChunk());
		indices->submitData();
	}

	void ChunkRenderUnit::ChangeVertices(std::vector<Vertex>& p_vertices)
	{
		vertices->flush();
		vertices->addBatchData(p_vertices);
		vertices->submitData();

		indices->flush();
		indices->addBatchData(GetIndicesForChunk());
		indices->submitData();
	}

	std::vector<unsigned int> ChunkRenderUnit::GetIndicesForChunk()
	{
		std::vector<unsigned int> indices;

		for (int y = 0; y < chunkHeight-1; y++)
		{
			for (int x = 0; x < chunkWidth-1; x++)
			{
				std::cout << "called\n";
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
				for (unsigned int i : indices)
				{
					std::cout << i << " - \n";
				}
			}
		}
		return indices;
	}
}
