#include "WorldWater.h"
#include "Camera.h"
#include "ChunkRenderUnit.h"

namespace WorldMaker
{
    float WorldWater::s_yPos = -30.0f;
    std::vector<Vertex> WorldWater::vertices;
    SSBOUPtr<unsigned int> WorldWater::m_indices = std::make_unique<SSBO<unsigned int>>(baseIndexCount, GL_DYNAMIC_STORAGE_BIT);
    SSBOUPtr<Vertex> WorldWater::m_vertices = std::make_unique<SSBO<Vertex>>(baseVertexCount, GL_DYNAMIC_STORAGE_BIT);
    void WorldWater::Init()
    {
        m_indices->pushData({0, 1, 3, 0, 3, 2});
    }
    void WorldWater::UpdateWaterTransform(ChunkGeneration::ChunkArray chunks)
    {
        int radius = ChunkRenderUnit::s_chunkSide * chunks.size();

        Vertex vertexA;
        vertexA.m_position = Camera::Position()+glm::vec3(-radius, 0, -radius);
        vertexA.m_position.y = s_yPos;

        Vertex vertexB;
        vertexB.m_position = Camera::Position()+glm::vec3(radius, 0, -radius);
        vertexB.m_position.y = s_yPos;

        Vertex vertexC;
        vertexC.m_position = Camera::Position()+glm::vec3(-radius, 0, radius);
        vertexC.m_position.y = s_yPos;

        Vertex vertexD;
        vertexD.m_position = Camera::Position()+glm::vec3(radius, 0, radius);
        vertexD.m_position.y = s_yPos;

        vertices.clear();
        vertices.push_back(vertexA);
        vertices.push_back(vertexB);
        vertices.push_back(vertexC);
        vertices.push_back(vertexD);
    }
}
