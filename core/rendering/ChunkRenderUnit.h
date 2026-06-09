#pragma once

#include "ResourceManager.h"
#include "ShaderProgram.h"
#include "VertexArray.h"
#include "Texture2D.h"
#include "RenderingConstants.h"
#include "SSBO.h"
#include "Material.h"

#include <memory>
#include <vector>

namespace WorldMaker
{
    struct Vertex;
    class Material;
	struct ChunkRenderUnit
	{
		static int chunkWidth; // Amount of pixels per chunk
		static int chunkHeight; // Amount of pixels per chunk
		ChunkRenderUnit(std::vector<Vertex>& p_vertices);
		void ChangeVertices(std::vector<Vertex>& p_vertices);
		static std::vector<unsigned int> GetIndicesForChunk();
		const ShaderProgramType shaderProgramType = ShaderProgramType::terrain;
		VertexArrayUPtr m_vertexArray = std::make_unique<VertexArray>();
		SSBOUPtr<Vertex> m_vertices = std::make_unique<SSBO<Vertex>>(maxVertexCount, GL_DYNAMIC_STORAGE_BIT);
		SSBOUPtr<unsigned int> m_indices = std::make_unique<SSBO<unsigned int>>(maxIndexCount, GL_DYNAMIC_STORAGE_BIT);
		Material* m_material;

		template<typename T>
        void resizeSSBO(SSBOUPtr<T>& ssbo, bool batchExceedsCapacity, unsigned int elementsToSupport)
        {
            unsigned int amplifier = batchExceedsCapacity ? ssboOversizeMultiplier : ssboAmplifier;
			SSBOUPtr<T> newSSBO = std::make_unique<SSBO<T>>((ssbo->maxElements())*amplifier, GL_DYNAMIC_STORAGE_BIT);
			newSSBO.get()->m_data = ssbo.get()->m_data;
			newSSBO.get()->m_currentBytes = ssbo.get()->m_currentBytes;

			GLCall(glBindBuffer(GL_COPY_WRITE_BUFFER, newSSBO.get()->glName()));
			GLCall(glBindBuffer(GL_COPY_READ_BUFFER, ssbo.get()->glName()));

			GLCall(glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, ssbo->maxSize()));
			ssbo = std::move(newSSBO);
			return;
		}
	};
}
