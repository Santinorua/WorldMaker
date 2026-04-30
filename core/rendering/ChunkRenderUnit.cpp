#include "ChunkRenderUnit.h"
#include "Vertex.h"

namespace WorldMaker
{
	const int ChunkRenderUnit::chunkWidth = 100; // Amount of pixels per chunk
	const int ChunkRenderUnit::chunkHeight = 100; // Amount of pixels per chunk

	ChunkRenderUnit::ChunkRenderUnit(std::vector<Vertex>& vertices)
	{
	    std::vector<unsigned int > indices = GetIndicesForChunk(); // TODO: Change so no need to calculate indices every time
       	if (m_vertices->checkIfPushIsBiggerThanMaxSize(vertices.size())) resizeSSBO(m_vertices, true);
       	if (m_indices->checkIfPushIsBiggerThanMaxSize(indices.size())) resizeSSBO(m_indices, true);

        if (!m_vertices->checkIfEnoughSpaceForPush(vertices.size())) m_vertices->pushBatchData(vertices);
		m_vertices->pushBatchData(vertices);
		m_vertices->submitData();

		if (!m_indices->checkIfEnoughSpaceForPush(indices.size())) resizeSSBO(m_indices, false);
		m_indices->pushBatchData(indices);
		m_indices->submitData();
	}

	void ChunkRenderUnit::ChangeVertices(std::vector<Vertex>& p_vertices)
	{
		m_vertices->flush();
		m_vertices->pushBatchData(p_vertices);
		m_vertices->submitData();

		m_indices->flush();
		m_indices->pushBatchData(GetIndicesForChunk());
		m_indices->submitData();
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
