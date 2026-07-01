#pragma once

#include "ResourceManager.h"
#include "ShaderProgram.h"
#include "VertexArray.h"
#include "Texture2D.h"
#include "RenderingConstants.h"
#include "SSBO.h"
#include "Material.h"
#include "Vertex.h"

#include <memory>
#include <vector>

namespace WorldMaker
{
    class Material;
	struct ChunkRenderUnit
	{
		static int s_chunkSide; // Amount of pixels per chunk
		double m_tallestPoint = 0;
		double m_lowestPoint = 0;
		ChunkRenderUnit(std::vector<Vertex>& p_vertices, double tallestPoint, double lowestPoint);
		glm::vec3 center();
		inline glm::vec3 minPoint() { return center()-glm::vec3(s_chunkSide/2, m_lowestPoint, s_chunkSide/2);}
		inline glm::vec3 maxPoint() { return center()+glm::vec3(s_chunkSide/2, m_tallestPoint, s_chunkSide/2);}
		void ChangeVertices(std::vector<Vertex>& p_vertices);
		static std::vector<unsigned int> GetIndicesForChunk();
		const ShaderProgramType shaderProgramType = ShaderProgramType::terrain;
		VertexArrayUPtr m_vertexArray = std::make_unique<VertexArray>();
		SSBOUPtr<Vertex> m_vertices = std::make_unique<SSBO<Vertex>>(maxVertexCount, GL_DYNAMIC_STORAGE_BIT);
		SSBOUPtr<unsigned int> m_indices = std::make_unique<SSBO<unsigned int>>(maxIndexCount, GL_DYNAMIC_STORAGE_BIT);

	};
}
