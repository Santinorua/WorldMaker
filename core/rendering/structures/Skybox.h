#pragma once

#include "ShaderProgram.h"
#include "VertexArray.h"
#include "Vertex.h"
#include "Cubemap.h"

namespace WorldMaker
{
    class Skybox
	{
	public:
	    static ShaderProgramSPtr s_shaderProgram;
		static VertexArrayUPtr s_vertexArray;
	    static bool s_inited;
		static CubemapSPtr s_cubemap;
		static SSBOUPtr<SkyboxVertex> s_verticesSSBO;
		static SSBOUPtr<unsigned int> s_indicesSSBO;

		static std::vector<SkyboxVertex> s_vertices;
		static std::vector<unsigned int> s_indices;

		static void Init();
	};
}
