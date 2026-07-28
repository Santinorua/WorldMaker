#include "WorldExporter.h"
#include "SSBO.h"

namespace WorldMaker
{
    void WorldExporter::ExportWorld(std::vector<ChunkRenderUnitUPtr> chunks)
    {

    }

    void WorldExporter::ExportChunkToGLB(ChunkRenderUnit* chunk)
    {
        tinygltf::Model model;
        model.asset.version = "2.0";
        model.asset.generator = "WorldMaker";

        std::vector<float> positions = BuildPositionBuffer(chunk->m_vertices->m_data);

        tinygltf::Buffer bufferPos;
        tinygltf::BufferView viewPos;
        tinygltf::Accessor accessorPos;

        SetVerticesForModel(positions, bufferPos, viewPos, accessorPos);

        model.buffers.push_back(bufferPos);
        model.bufferViews.push_back(viewPos);
        model.accessors.push_back(accessorPos);

        tinygltf::Buffer bufferIndices;
        tinygltf::BufferView viewIndices;
        tinygltf::Accessor accessorIndices;

        SetIndicesForModel(chunk->m_indices->m_data, bufferIndices, viewIndices, accessorIndices);

        model.buffers.push_back(bufferIndices);
        model.bufferViews.push_back(viewIndices);
        model.accessors.push_back(accessorIndices);

        tinygltf::Primitive primitive;
        primitive.attributes["POSITION"] = 0;
        primitive.indices = 1;
        primitive.mode = TINYGLTF_MODE_TRIANGLES;

        tinygltf::Mesh mesh;
        mesh.primitives.push_back(primitive);
        model.meshes.push_back(mesh);

        tinygltf::Node node;
        node.mesh = 0;
        node.name = "test";

        model.nodes.push_back(node);

        tinygltf::Scene scene;
        scene.nodes.push_back(0);

        model.scenes.push_back(scene);
        model.defaultScene = 0;

        tinygltf::TinyGLTF writer;
        bool ok = writer.WriteGltfSceneToFile(
            &model,
            "/home/coolfede97/chunk.glb",
            true,
            true,
            true,
            true
        );

        if (!ok) std::cerr << "Failed when exporting glb\n";
        else std::cout << "Chunk exported successfully!\n";
    }

    void WorldExporter::SetVerticesForModel(std::vector<float>& positions, tinygltf::Buffer& bufferPos, tinygltf::BufferView& viewPos, tinygltf::Accessor& accessorPos)
    {
        size_t posByteLength = positions.size() * sizeof(float);
        bufferPos.data.resize(posByteLength);
        memcpy(bufferPos.data.data(), positions.data(), posByteLength);

        viewPos.buffer = 0;
        viewPos.byteOffset = 0;
        viewPos.byteLength = posByteLength;
        viewPos.target = TINYGLTF_TARGET_ARRAY_BUFFER;

        accessorPos.bufferView = 0;
        accessorPos.byteOffset = 0;
        accessorPos.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
        accessorPos.count = positions.size() / 3;
        accessorPos.type = TINYGLTF_TYPE_VEC3;

        float minX = FLT_MAX, minY = FLT_MAX, minZ = FLT_MAX;
        float maxX = -FLT_MAX, maxY = -FLT_MAX, maxZ = -FLT_MAX;

        for (unsigned int i = 0; i < positions.size(); i+=3)
        {
            minX = std::min(minX, positions[i]);
            minY = std::min(minY, positions[i+1]);
            minZ = std::min(minZ, positions[i+2]);

            maxX = std::max(maxX, positions[i]);
            maxY = std::max(maxY, positions[i+1]);
            maxZ = std::max(maxZ, positions[i+2]);
        }
        accessorPos.minValues = { minX, minY, minZ};
        accessorPos.maxValues = { maxX, maxY, maxZ};
    }

    void WorldExporter::SetIndicesForModel(std::vector<unsigned int>& indices, tinygltf::Buffer& bufferIndices, tinygltf::BufferView& viewIndices, tinygltf::Accessor& accessorIndices)
    {
        size_t indicesByteLength = indices.size() * sizeof(unsigned int);
        bufferIndices.data.resize(indicesByteLength);
        memcpy(bufferIndices.data.data(), indices.data(), indicesByteLength);

        viewIndices.buffer = 1;
        viewIndices.byteOffset = 0;
        viewIndices.byteLength = indicesByteLength;
        viewIndices.target = TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER;

        accessorIndices.bufferView = 1;
        accessorIndices.byteOffset = 0;
        accessorIndices.componentType = TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT;
        accessorIndices.count = indices.size();
        accessorIndices.type = TINYGLTF_TYPE_SCALAR;
    }
    std::vector<float> WorldExporter::BuildPositionBuffer(const std::vector<Vertex>& vertices)
    {
        std::vector<float> positions;
        positions.reserve(vertices.size()*3);
        for (Vertex vertex : vertices)
        {
            positions.push_back((float)vertex.m_position.x);
            positions.push_back((float)vertex.m_position.y);
            positions.push_back((float)vertex.m_position.z);
        }
        return positions;
    }
}
