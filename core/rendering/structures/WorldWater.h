#pragma once

#include "Vertex.h"
#include "ChunkGeneration.h"
#include <vector>

namespace WorldMaker
{
    struct WorldWater
    {
    public:
        static float s_yPos;
        static void UpdateWaterTransform(ChunkGeneration::ChunkArray chunks);
        static SSBOUPtr<unsigned int> s_indices;
        static SSBOUPtr<Vertex> s_vertices;
        static const ShaderProgramType s_shaderProgramType;
		static VertexArrayUPtr s_vertexArray;
        static void Init();
    };
}
