#include "ChunkUnion.h"
#include "ChunkRenderUnit.h"
#include "Vertex.h"

namespace WorldMaker
{
	ChunkUnion::ChunkUnion(std::vector<Vertex>& vertices)
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

	void ChunkUnion::ChangeVertices(std::vector<Vertex>& vertices)
	{
	    while (m_vertices->checkIfPushIsBiggerThanMaxSize(vertices.size())) resizeSSBO(m_vertices, true, vertices.size());
		m_vertices->flush();
		m_vertices->pushBatchData(vertices);
		m_vertices->submitData();

		std::vector<unsigned int> indices = ChunkUnion::GetIndicesForChunk();

		while (m_indices->checkIfPushIsBiggerThanMaxSize(indices.size())) resizeSSBO(m_indices, true, indices.size());
		m_indices->flush();
		m_indices->pushBatchData(indices);
		m_indices->submitData();
	}

	ChunkUnion::ChunkUnion(const std::vector<Vertex>& a, const std::vector<Vertex>& b)
	{
		std::vector<Vertex> vertices{static_cast<size_t>(ChunkRenderUnit::chunkHeight * 2)};
		for (int i = 0; i < ChunkRenderUnit::chunkHeight; i++) {
			if (i % 2 == 0) {
				vertices.push_back(a[i * ChunkRenderUnit::chunkHeight]);
				continue;
			}
			vertices.push_back(b[(i+1) * ChunkRenderUnit::chunkHeight - 1]);
		}

		std::vector<unsigned int> indices;

		for (int i = 0; i < ChunkRenderUnit::chunkHeight - 2; i += 2) {
			indices.push_back(i);
			indices.push_back(i+1);
			indices.push_back(i+2);

			indices.push_back(i);
			indices.push_back(i+2);
			indices.push_back(i+3);
		}

       	while (m_vertices->checkIfPushIsBiggerThanMaxSize(vertices.size())) resizeSSBO(m_vertices, true, vertices.size());
       	while (m_indices->checkIfPushIsBiggerThanMaxSize(indices.size())) resizeSSBO(m_indices, true, indices.size());

        while (!m_vertices->checkIfEnoughSpaceForPush(vertices.size())) resizeSSBO(m_vertices, false, vertices.size());
		m_vertices->pushBatchData(vertices);
		m_vertices->submitData();

		while (!m_indices->checkIfEnoughSpaceForPush(indices.size())) resizeSSBO(m_indices, false, indices.size());
		m_indices->pushBatchData(indices);
		m_indices->submitData();
	}
}
