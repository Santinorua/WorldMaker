#pragma once

#include "ArrayTexture2D.h"
#include "BufferBuilder.h"
#include "ChunkRenderUnit.h"

#include <vector>
#include "MeshMaterial.h"
#include "tiny_gltf.h"

namespace WorldMaker
{
    class BufferBuilder;
    class WorldExporter
    {
    public:
        static void ExportWorld(std::vector<ChunkRenderUnitUPtr> chunks);
        static void ExportChunkToGLB(ChunkRenderUnit* chunk);
        static void ExportModelToGLB(ModelSPtr model, const std::string& outPath);
        static int AddTextureImage(tinygltf::Model& model, BufferBuilder& bufferBuilder, Texture2DSPtr texture);
        static int AddTexture(tinygltf::Model& model, int imageIndex);
        static int AddMaterial(tinygltf::Model& model, BufferBuilder& bufferBuilder, MeshMaterialSPtr meshMaterial);
        static int SetPositionsForModel(size_t offsetInBuffer, tinygltf::Model& model, std::vector<float>& positions);
        static int SetIndicesForModel(size_t offsetInBuffer, tinygltf::Model& model, std::vector<unsigned int>& indices);
        static int SetAttributeForModel(size_t offsetInBuffer, tinygltf::Model& model, size_t elementCount,
            int type);

        static std::vector<float> BuildPositionBuffer(const std::vector<Vertex>& vertices);
        static std::vector<float> BuildColorBuffer(const std::vector<Vertex>& vertices);
        static std::vector<float> BuildNormalBuffer(const std::vector<Vertex>& vertices);
        static std::vector<float> BuildUVBuffer(const std::vector<Vertex>& vertices);
    };
}
