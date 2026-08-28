#include "WorldWater.h"
#include "Camera.h"
#include "ChunkRenderUnit.h"
#include "SSBO.h"
namespace WorldMaker
{
    float WorldWater::s_yPos = -40.0f;
    SSBOUPtr<unsigned int> WorldWater::s_indices = nullptr;
    SSBOUPtr<Vertex> WorldWater::s_vertices = nullptr;
    VertexArrayUPtr WorldWater::s_vertexArray = nullptr;
    ShaderProgramType const WorldWater::s_shaderProgramType = ShaderProgramType::water;
    void WorldWater::Init()
    {
        s_indices = std::make_unique<SSBO<unsigned int>>(baseIndexCount, GL_DYNAMIC_STORAGE_BIT);
        s_vertices = std::make_unique<SSBO<Vertex>>(baseVertexCount, GL_DYNAMIC_STORAGE_BIT);
        s_vertexArray = std::make_unique<VertexArray>();
        s_indices->pushData({0, 1, 3, 0, 3, 2});
        s_indices->submitData();
    }
    void WorldWater::UpdateWaterTransform(ChunkGeneration::ChunkArray chunks)
    {
        s_vertices->flush();

        int radius = ChunkRenderUnit::s_chunkSide * chunks.size();
        float test = 10.0f;
        Vertex vertexA;
        vertexA.m_position = Camera::Position()+glm::vec3(-radius, 0, -radius);
        vertexA.m_position.y = s_yPos;
        vertexA.m_uv = {vertexA.m_position.x / test,
            vertexA.m_position.z / test};
        vertexA.m_normal = {0,1,0};
        vertexA.m_materialLayer = 1;

        Vertex vertexB;
        vertexB.m_position = Camera::Position()+glm::vec3(radius, 0, -radius);
        vertexB.m_position.y = s_yPos;
        vertexB.m_uv = {vertexB.m_position.x / test,
            vertexB.m_position.z / test};
        vertexB.m_normal = {0,1,0};
        vertexB.m_materialLayer = 1;

        Vertex vertexC;
        vertexC.m_position = Camera::Position()+glm::vec3(-radius, 0, radius);
        vertexC.m_position.y = s_yPos;
        vertexC.m_uv = { vertexC.m_position.x / test,
           vertexC.m_position.z / test};
        vertexC.m_normal = {0,1,0};
        vertexC.m_materialLayer = 1;

        Vertex vertexD;
        vertexD.m_position = Camera::Position()+glm::vec3(radius, 0, radius);
        vertexD.m_position.y = s_yPos;
        vertexD.m_uv = {vertexD.m_position.x / test,
            vertexD.m_position.z / test};
        vertexD.m_normal = {0,1,0};
        vertexD.m_materialLayer = 1;

        s_vertices->flush();
        s_vertices->pushData(vertexA);
        s_vertices->pushData(vertexB);
        s_vertices->pushData(vertexC);
        s_vertices->pushData(vertexD);
        s_vertices->submitData();
    }
}
