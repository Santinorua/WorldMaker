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
        static glm::ivec2 s_lastPlayerChunkPos;
        static void UpdateWaterTransform(int renderDistance);
        static SSBOUPtr<unsigned int> s_indices;
        static SSBOUPtr<MeshVertex> s_vertices;
        static const ShaderProgramType s_shaderProgramType;
		static VertexArrayUPtr s_vertexArray;
        static void Init();
    };
}
