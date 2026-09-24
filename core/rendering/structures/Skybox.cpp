#include "Skybox.h"
#include "Vertex.h"

namespace WorldMaker
{
    bool Skybox::s_inited = false;
    ShaderProgramSPtr Skybox::s_shaderProgram = nullptr;
    VertexArrayUPtr Skybox::s_vertexArray = nullptr;
    SSBOUPtr<SkyboxVertex> Skybox::s_verticesSSBO = nullptr;
    SSBOUPtr<unsigned int> Skybox::s_indicesSSBO = nullptr;
    CubemapSPtr Skybox::s_cubemap = nullptr;

    std::vector<SkyboxVertex> Skybox::s_vertices = SkyboxVertex::CreateCube();

    std::vector<unsigned int> Skybox::s_indices = {
        // Back (+Z)
        4, 6, 5,
        6, 4, 7,

        // Front (-Z)
        0, 1, 2,
        2, 3, 0,

        // Left (-X)
        0, 3, 7,
        7, 4, 0,

        // Right (+X)
        1, 5, 6,
        6, 2, 1,

        // Top (+Y)
        3, 2, 6,
        6, 7, 3,

        // Bottom (-Y)
        0, 4, 5,
        5, 1, 0
    };
}
