#include "ChunkRenderUnit.h"
#include "Vertex.h"

namespace WorldMaker
{
	int ChunkRenderUnit::chunkSide = 64; // Amount of pixels per chunk
	int ChunkRenderUnit::chunkHeight = 256; // Amount of pixels per chunk

	glm::vec3 ChunkRenderUnit::center()
	{
	    glm::vec3 point = m_vertices->m_data[0].m_position; // First vertex pos
		point.y = chunkHeight/2.0; // Y coordinate in the center
		point+=glm::vec3(chunkSide/2 ,0 ,chunkSide/2); // X and Z coordinates in the center
	    return point;
	}

	ChunkRenderUnit::ChunkRenderUnit(std::vector<Vertex>& vertices)
	{
	    std::vector<unsigned int > indices = GetIndicesForChunk(); // TODO: Change so no need to calculate indices every time
       	while (m_vertices->checkIfPushIsBiggerThanMaxSize(vertices.size())) resizeSSBO(m_vertices, true, vertices.size());
       	while (m_indices->checkIfPushIsBiggerThanMaxSize(indices.size())) resizeSSBO(m_indices, true, indices.size());

        while (!m_vertices->checkIfEnoughSpaceForPush(vertices.size())) resizeSSBO(m_vertices, false, vertices.size());
		m_vertices->pushBatchData(vertices);
		m_vertices->submitData();

		while (!m_indices->checkIfEnoughSpaceForPush(indices.size())) resizeSSBO(m_indices, false, indices.size());
		m_indices->pushBatchData(indices);
		m_indices->submitData();
	}

	void ChunkRenderUnit::ChangeVertices(std::vector<Vertex>& vertices)
	{
	    while (m_vertices->checkIfPushIsBiggerThanMaxSize(vertices.size())) resizeSSBO(m_vertices, true, vertices.size());
		m_vertices->flush();
		m_vertices->pushBatchData(vertices);
		m_vertices->submitData();

		std::vector<unsigned int> indices = ChunkRenderUnit::GetIndicesForChunk();

		while (m_indices->checkIfPushIsBiggerThanMaxSize(indices.size())) resizeSSBO(m_indices, true, indices.size());
		m_indices->flush();
		m_indices->pushBatchData(indices);
		m_indices->submitData();
	}

	std::vector<unsigned int> ChunkRenderUnit::GetIndicesForChunk()
	{
		std::vector<unsigned int> indices;

		for (int y = 0; y < chunkSide-1; y++)
		{
			for (int x = 0; x < chunkSide-1; x++)
			{
				unsigned int bottomLeft = y * chunkSide + x;
				unsigned int bottomRight = y * chunkSide + (x + 1);
				unsigned int topLeft = (y+1) * chunkSide + x;
				unsigned int topRight = (y+1) * chunkSide + (x +1);

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
