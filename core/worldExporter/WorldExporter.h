#pragma once

#include "ChunkRenderUnit.h"

#include <vector>
// #define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"

namespace WorldMaker
{
    class WorldExporter
    {
    public:
        static void ExportWorld(std::vector<ChunkRenderUnitUPtr> chunks);
        static void ExportChunkToGLB(ChunkRenderUnit* chunk);
        static void SetVerticesForModel(std::vector<float>& positions, tinygltf::Buffer& bufferPos, tinygltf::BufferView& viewPos, tinygltf::Accessor& accessorPos);
        static void SetIndicesForModel(std::vector<unsigned int>& indices, tinygltf::Buffer& bufferIndices, tinygltf::BufferView& viewIndices, tinygltf::Accessor& accessorIndices);
        static std::vector<float> BuildPositionBuffer(const std::vector<Vertex>& vertices);
    };
}
