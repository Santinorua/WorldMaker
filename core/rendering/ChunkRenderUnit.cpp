#include "ChunkRenderUnit.h"
#include "Vertex.h"

namespace WorldMaker
{
	int ChunkRenderUnit::s_chunkSide = 64; // Amount of pixels per chunk

	glm::vec3 ChunkRenderUnit::center()
	{
	    glm::vec3 point = m_vertices->m_data[0].m_position; // First vertex pos
		point.y = (m_tallestPoint+m_lowestPoint)/2; // Y coordinate in the center
		point+=glm::vec3(s_chunkSide/2 ,0 ,s_chunkSide/2); // X and Z coordinates in the center
	    return point;
	}

	ChunkRenderUnit::ChunkRenderUnit(std::vector<Vertex>& vertices, double tallestPoint, double lowestPoint, ChunkModels chunkModels)
	{
	    m_models = chunkModels;
        m_lowestPoint = std::min(lowestPoint, chunkModels.lowestPoint);
        m_tallestPoint = std::max(tallestPoint, chunkModels.tallestPoint);
        std::vector<unsigned int > indices = GetIndicesForChunk(); // TODO: Change so no need to calculate indices every time
		m_vertices->pushData(vertices);
		m_vertices->submitData();

		m_indices->pushData(indices);
		m_indices->submitData();
	}

	std::vector<unsigned int> ChunkRenderUnit::GetIndicesForChunk()
	{
		std::vector<unsigned int> indices;

		for (int y = 0; y < s_chunkSide-1; y++)
		{
			for (int x = 0; x < s_chunkSide-1; x++)
			{
				unsigned int bottomLeft = y * s_chunkSide + x;
				unsigned int bottomRight = y * s_chunkSide + (x + 1);
				unsigned int topLeft = (y+1) * s_chunkSide + x;
				unsigned int topRight = (y+1) * s_chunkSide + (x +1);

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
