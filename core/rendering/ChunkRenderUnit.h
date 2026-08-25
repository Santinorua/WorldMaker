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
	    double tallestPoint = 0;
		double lowestPoint = 0;
	    std::unordered_map<unsigned int, std::pair<ModelSPtr, MatricesSSBO>> m_modelInstancesSSBO = {};
		void addInstance(const std::string& modelPath, glm::vec3 pos, glm::quat rot = glm::identity<glm::quat>(), glm::vec3 scale = {1,1,1});
	};
	struct ChunkRenderUnit
	{
		static const int MAX_LOD = 4;
		static int s_chunkRes; // Amount of pixels per chunk
		static double s_chunkScale;
		double m_tallestPoint = 0;
		double m_lowestPoint = 0;
		int m_current_lod = 0;
		ChunkRenderUnit(std::vector<Vertex>& vertices, double tallestPoint, double lowestPoint, ChunkModels chunkModels, int lod);
		glm::vec3 center();

		void uploadLOD(std::vector<Vertex>& vertices, int lod);
		void setLOD(int lod);
		bool hasLOD(int lod);

		inline glm::vec3 minPoint() { return glm::vec3(center().x-s_chunkRes/2.0, m_lowestPoint, center().z - s_chunkRes/2.0);}
		inline glm::vec3 maxPoint() { return glm::vec3(center().x+s_chunkRes/2.0, m_tallestPoint, center().z + s_chunkRes/2.0);}

		static std::vector<unsigned int> GetIndicesForChunk(int lod);

		const ShaderProgramType shaderProgramType = ShaderProgramType::terrain;
		VertexArrayUPtr m_vertexArray = std::make_unique<VertexArray>();
		std::unordered_map<int, SSBOUPtr<Vertex>> m_vertices; //= { std::make_unique<SSBO<Vertex>>(baseVertexCount, GL_DYNAMIC_STORAGE_BIT) };
		std::unordered_map<int, SSBOUPtr<unsigned int>> m_indices; //= std::make_unique<SSBO<unsigned int>>(baseIndexCount, GL_DYNAMIC_STORAGE_BIT);
		ChunkModels m_models = {};
	};
	using ChunkRenderUnitUPtr = std::unique_ptr<ChunkRenderUnit>;
}
