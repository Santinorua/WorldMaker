#pragma once

#include "ResourceManager.h"
#include "ShaderProgram.h"
#include "VertexArray.h"
#include "RenderingConstants.h"
#include "SSBO.h"
#include "Vertex.h"
#include "SSBO.h"
#include "Model.h"

#include <memory>
#include <vector>

#include <glm/gtc/quaternion.hpp>

namespace WorldMaker
{
    class TerrainMaterial;
	struct ChunkModels
	{
	    std::unordered_map<unsigned int, std::pair<ModelSPtr, MatricesSSBO>> m_modelInstancesSSBO = {};
		void addInstance(const std::string& modelPath, glm::vec3 pos, glm::quat rot = glm::identity<glm::quat>(), glm::vec3 scale = {1,1,1});
	};
	struct ChunkRenderUnit
	{
		static int s_chunkSide; // Amount of pixels per chunk
		double m_tallestPoint = 0;
		double m_lowestPoint = 0;
		ChunkRenderUnit(std::vector<Vertex>& p_vertices, double tallestPoint, double lowestPoint, ChunkModels chunkModels);
		glm::vec3 center();
		inline glm::vec3 minPoint() { return glm::vec3(center().x-s_chunkSide/2.0, m_lowestPoint, center().z - s_chunkSide/2.0);}
		inline glm::vec3 maxPoint() { return glm::vec3(center().x+s_chunkSide/2.0, m_tallestPoint, center().z + s_chunkSide/2.0);}
		static std::vector<unsigned int> GetIndicesForChunk();
		const ShaderProgramType shaderProgramType = ShaderProgramType::terrain;
		VertexArrayUPtr m_vertexArray = std::make_unique<VertexArray>();
		SSBOUPtr<Vertex> m_vertices = std::make_unique<SSBO<Vertex>>(baseVertexCount, GL_DYNAMIC_STORAGE_BIT);
		SSBOUPtr<unsigned int> m_indices = std::make_unique<SSBO<unsigned int>>(baseIndexCount, GL_DYNAMIC_STORAGE_BIT);
		ChunkModels m_models = {};
	};
	using ChunkRenderUnitUPtr = std::unique_ptr<ChunkRenderUnit>;
}
