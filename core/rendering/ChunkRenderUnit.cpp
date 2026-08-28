#include "ChunkRenderUnit.h"
#include "Vertex.h"


namespace WorldMaker
{
	int ChunkRenderUnit::s_chunkRes = 64; // Amount of pixels per chunk
	double ChunkRenderUnit::s_chunkScale = 64; // Amount of pixels per chunk

	glm::vec3 ChunkRenderUnit::center()
	{
	    glm::vec3 point = m_vertices[m_current_lod]->m_data[0].m_position; // First vertex pos
		if (m_vertices.contains(0)) {
			point = m_vertices[0]->m_data[0].m_position; // First vertex pos
		}
		point.y = (m_tallestPoint+m_lowestPoint)/2; // Y coordinate in the center
		point+=glm::vec3((s_chunkRes * s_chunkScale) /2 ,0 ,(s_chunkRes * s_chunkScale)/2); // X and Z coordinates in the center
	    return point;
	}

	bool ChunkRenderUnit::hasLOD(int lod) {
		return m_vertices.contains(lod);
	}

	void ChunkRenderUnit::setLOD(int lod)
	{
		m_current_lod = lod;
		m_vertices[m_current_lod]->submitData();
		m_indices[m_current_lod]->submitData();
		// TODO: Delete buffers when unused
	}

	void ChunkRenderUnit::uploadLOD(std::vector<Vertex>& vertices, int lod, double tallest_point, double lowest_point)
	{
		std::vector<unsigned int> indices = GetIndicesForChunk(lod);
		m_vertices.insert({lod, std::make_unique<SSBO<Vertex>>(baseVertexCount, GL_DYNAMIC_STORAGE_BIT)});
		m_indices.insert({lod, std::make_unique<SSBO<unsigned int>>(baseIndexCount, GL_DYNAMIC_STORAGE_BIT)});

		m_vertices[lod]->pushData(vertices);

		m_indices[lod]->pushData(indices);

		m_tallestPoint = tallest_point;
		m_lowestPoint = lowest_point;
	}

	ChunkRenderUnit::ChunkRenderUnit(std::vector<Vertex>& vertices, double tallestPoint, double lowestPoint, ChunkModels chunkModels, int lod)
	{
		std::vector<unsigned int> indices = GetIndicesForChunk(lod);
		m_vertices.insert({lod, std::make_unique<SSBO<Vertex>>(baseVertexCount, GL_DYNAMIC_STORAGE_BIT)});
		m_indices.insert({lod, std::make_unique<SSBO<unsigned int>>(baseIndexCount, GL_DYNAMIC_STORAGE_BIT)});

		m_indices[lod]->pushData(indices);

		m_vertices[lod]->pushData(vertices);

		setLOD(lod);

		m_models = chunkModels;
		m_lowestPoint = std::min(lowestPoint, chunkModels.lowestPoint);
		m_tallestPoint = std::max(tallestPoint, chunkModels.tallestPoint);
	}

	std::vector<unsigned int> ChunkRenderUnit::GetIndicesForChunk(int lod)
	{
        std::vector<unsigned int> indices;
		int side = (s_chunkRes >> lod) + 1;
        for (int y = 0; y < side-1; y++)
        {
            for (int x = 0; x < side-1; x++)
            {
                unsigned int bottomLeft = y * side + x;
                unsigned int bottomRight = y * side + (x + 1);
                unsigned int topLeft = (y+1) * side + x;
                unsigned int topRight = (y+1) * side + (x +1);

                indices.push_back(bottomLeft);
                indices.push_back(topRight);
                indices.push_back(bottomRight);

                indices.push_back(bottomLeft);
                indices.push_back(topLeft);
                indices.push_back(topRight);
            }
        }
        return indices;
	}

	void ChunkModels::addInstance(const std::string& modelPath, glm::vec3 pos, glm::quat rot, glm::vec3 scale)
	{
        ModelSPtr model = ResourceManager::LoadModel(modelPath);
        auto& ssbo = m_modelInstancesSSBO[model->instanceId()].second;
        if (!ssbo)
        {
            ssbo = std::make_shared<SSBO<glm::mat4>>(baseVertexCount, GL_DYNAMIC_STORAGE_BIT);
        }
        glm::mat4 modelMat =
            glm::translate(glm::mat4(1), pos) *
            glm::mat4_cast(rot) *
            glm::scale(glm::mat4(1), scale);
        ssbo->pushData(modelMat);
        ssbo->submitData();
        m_modelInstancesSSBO[model->instanceId()].first = model;
        tallestPoint = std::max(tallestPoint, pos.y + model->tallestPoint() * scale.y);
        lowestPoint = std::max(lowestPoint, pos.y + model->lowestPoint() * scale.y);
	}
}
