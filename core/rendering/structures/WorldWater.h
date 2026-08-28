#pragma once

#include "Vertex.h"
#include "ChunkGeneration.h"
#include <vector>

namespace WorldMaker
{
    struct WorldWater
    {
        static float s_yPos;
        static std::vector<Vertex> vertices;
        static void UpdateWaterTransform(ChunkGeneration::ChunkArray chunks);
        static SSBOUPtr<unsigned int> m_indices;
        static SSBOUPtr<Vertex> m_vertices;
        static void Init();
    };
}
