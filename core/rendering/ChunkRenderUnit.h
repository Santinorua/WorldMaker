#pragma once

#include "ShaderProgram.h"
#include "VertexArray.h"
#include "Texture2D.h"
#include "RenderingConstants.h"

#include <vector>

namespace WorldMaker
{
	struct ChunkRenderUnit
	{
		static const int chunkWidth; // Amount of pixels per chunk
		static const int chunkHeight; // Amount of pixels per chunk
		ChunkRenderUnit(std::vector<Vertex>& p_vertices);
		void ChangeVertices(std::vector<Vertex>& p_vertices);
		static std::vector<unsigned int> GetIndicesForChunk();
		const ShaderProgramType shaderProgramType = ShaderProgramType::terrain;
		VertexArrayUPtr vertexArray = std::make_unique<VertexArray>();
		SSBOUPtr<Vertex> vertices = std::make_unique<SSBO<Vertex>>(maxVertexCount, GL_DYNAMIC_STORAGE_BIT);
		SSBOUPtr<unsigned int> indices = std::make_unique<SSBO<unsigned int>>(maxIndexCount, GL_DYNAMIC_STORAGE_BIT);
	};
}
