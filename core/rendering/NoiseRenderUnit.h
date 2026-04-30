#pragma once

#include "RenderingConstants.h"
#include "ShaderProgram.h"
#include "Texture2D.h"
#include "VertexArray.h"
#include "SSBO.h"
#include "Vertex.h"

namespace WorldMaker
{
	struct NoiseRenderUnit
	{
		NoiseRenderUnit(const int& p_width, const int& p_height, std::vector<double>& p_data);
		void ChangeNoise(const int& p_width, const int& p_height, std::vector<double>& p_data);
		const ShaderProgramType shaderProgramType = ShaderProgramType::noise;
		VertexArrayUPtr vertexArray = std::make_unique<VertexArray>();
		int width = 0;
		int height = 0;
		std::vector<double> colorVector = {};
		Texture2DSPtr noiseTex = nullptr;
		std::vector<Vertex> quad = Vertex::CreateUnitQuadWithAspectRatio(1,1);
		const std::vector<unsigned int> quadIndices=
		{
			0,1,2,0,2,3
		};
		SSBOUPtr<Vertex> vertices = std::make_unique<SSBO<Vertex>>(maxVertexCount, GL_DYNAMIC_STORAGE_BIT);
		SSBOUPtr<unsigned int> indices = std::make_unique<SSBO<unsigned int>>(maxIndexCount, GL_DYNAMIC_STORAGE_BIT);
	};
}
