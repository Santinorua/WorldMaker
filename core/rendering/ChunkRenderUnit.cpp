#include "ChunkRenderUnit.h"
#include "Vertex.h"

namespace WorldMaker
{
	int ChunkRenderUnit::s_chunkSide = 64; // Amount of pixels per chunk

	glm::vec3 ChunkRenderUnit::center()
	{
	    glm::vec3 point = m_vertices[m_current_lod]->m_data[0].m_position; // First vertex pos
		point.y = (m_tallestPoint+m_lowestPoint)/2; // Y coordinate in the center
		point+=glm::vec3(s_chunkSide/2 ,0 ,s_chunkSide/2); // X and Z coordinates in the center
	    return point;
	}

	void ChunkRenderUnit::setLOD(int lod)
	{
		m_current_lod = lod;
		m_vertices[m_current_lod]->submitData();
		m_indices[m_current_lod]->submitData();
	}

	ChunkRenderUnit::ChunkRenderUnit(std::vector<std::vector<Vertex>>& vertices, double tallestPoint, double lowestPoint, ChunkModels chunkModels, int lod)
	{
		for (int i = 0; i < vertices.size(); i++) {
			std::vector<unsigned int> indices = GetIndicesForChunk(i); // TODO: Change so no need to calculate indices every time

			m_vertices.push_back(std::make_unique<SSBO<Vertex>>(baseVertexCount, GL_DYNAMIC_STORAGE_BIT));
			m_indices.push_back(std::make_unique<SSBO<unsigned int>>(baseIndexCount, GL_DYNAMIC_STORAGE_BIT));

			m_vertices[i]->pushData(vertices[i]);

			m_indices[i]->pushData(indices);
		}

		setLOD(lod);

		m_models = chunkModels;
		m_lowestPoint = std::min(lowestPoint, chunkModels.lowestPoint);
		m_tallestPoint = std::max(tallestPoint, chunkModels.tallestPoint);
	}

	std::vector<unsigned int> ChunkRenderUnit::GetIndicesForChunk(int lod)
	{
        std::vector<unsigned int> indices;
		int side = (s_chunkSide >> lod) + (lod != 0);
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
