#include "WorldExporter.h"
#include "BufferBuilder.h"
#include "SSBO.h"
#include "stb_image_write.h"
#include "glm/common.hpp"

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
        std::vector<float> colors = BuildColorBuffer(chunk->m_vertices->m_data);
        std::vector<float> normals = BuildNormalBuffer(chunk->m_vertices->m_data);
        std::vector<float> uvs = BuildUVBuffer(chunk->m_vertices->m_data);
        std::vector<unsigned int>& indices = chunk->m_indices->m_data;
        BufferBuilder bufferBuilder;

        size_t posOffset = bufferBuilder.AddBlock<float>(positions);
        size_t idxOffset = bufferBuilder.AddBlock<unsigned int>(indices);
        size_t colorOffset = bufferBuilder.AddBlock<float>(colors);
        size_t normalOffset = bufferBuilder.AddBlock<float>(normals);
        size_t uvOffset = bufferBuilder.AddBlock<float>(uvs);

        int positionIdx = SetPositionsForModel(posOffset, model, positions);

        int indexIdx = SetIndicesForModel(idxOffset, model, indices);

        int colorIdx = SetAttributeForModel(
            colorOffset,
            model,
            colors.size()/4,
            TINYGLTF_TYPE_VEC4);

        int normalIdx = SetAttributeForModel(
            normalOffset,
            model,
            normals.size()/3,
            TINYGLTF_TYPE_VEC3);

        int uvIdx = SetAttributeForModel(
            uvOffset,
            model,
            uvs.size()/2,
            TINYGLTF_TYPE_VEC2);

        model.buffers.push_back(bufferBuilder.buffer);

        tinygltf::Primitive primitive;
        primitive.attributes["POSITION"] = positionIdx;
        primitive.indices = indexIdx;
        primitive.attributes["COLOR_0"] = colorIdx;
        primitive.attributes["NORMAL"] = normalIdx;
        primitive.attributes["TEXCOORD_0"] = uvIdx;

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

    int WorldExporter::AddTextureImage(tinygltf::Model& model, BufferBuilder& bufferBuilder, Texture2DSPtr texture)
    {
        std::vector<unsigned char> pngBytes;
        auto writeCallback = [](void* context, void* data, int size)
        {
            auto* buffer = reinterpret_cast<std::vector<unsigned char>*>(context);
            unsigned char* bytes = reinterpret_cast<unsigned char*>(data);
            buffer->insert(buffer->end(), bytes, bytes + size);
        };

        int channels = 4; // RGBA
        stbi_write_png_to_func(
            writeCallback,
            &pngBytes,
            texture->width(),
            texture->height(),
            channels,
            texture->m_unsignedCharLocalBuffer,
            texture->width() * channels);

        size_t offset = bufferBuilder.AddBlock<unsigned char>(pngBytes);

        tinygltf::BufferView view;
        view.buffer = 0;
        view.byteOffset = offset;
        view.byteLength = pngBytes.size();

        model.bufferViews.push_back(view);
        int viewIdx = (int)model.bufferViews.size()-1;

        tinygltf::Image image;
        image.mimeType = "image/png";
        image.width = texture->width();
        image.height = texture->height();
        image.bufferView = viewIdx;
        image.as_is = false;

        model.images.push_back(image);
        return (int)model.images.size()-1;
    }
    int WorldExporter::AddTexture(tinygltf::Model& model, int imageIndex)
    {
        tinygltf::Texture texture;
        texture.source = imageIndex;

        model.textures.push_back(texture);
        return (int)model.textures.size()-1;
    }

    int WorldExporter::AddMaterial(tinygltf::Model& model, BufferBuilder& bufferBuilder, MeshMaterialSPtr meshMaterial)
    {
        tinygltf::Material material;

        if (meshMaterial->m_diffuseTexture)
        {
            int imageIdx = AddTextureImage(model, bufferBuilder, meshMaterial->m_diffuseTexture);
            int textureIdx = AddTexture(model, imageIdx);

            material.pbrMetallicRoughness.baseColorTexture.index = textureIdx;
        }

        model.materials.push_back(material);
        return (int)model.materials.size()-1;
    }

    int WorldExporter::SetAttributeForModel(size_t offsetInBuffer, tinygltf::Model& model, size_t elementCount, int type)
    {

        size_t componentsPerElement;
        switch (type)
        {
            case TINYGLTF_TYPE_VEC2: componentsPerElement = 2; break;
            case TINYGLTF_TYPE_VEC3: componentsPerElement = 3; break;
            case TINYGLTF_TYPE_VEC4: componentsPerElement = 4; break;
            default: componentsPerElement = 1;
        }
        size_t byteLength = elementCount * componentsPerElement * sizeof(float);

        tinygltf::BufferView view;
        view.buffer = 0;
        view.byteOffset = offsetInBuffer;
        view.byteLength = byteLength;
        view.target = TINYGLTF_TARGET_ARRAY_BUFFER;

        tinygltf::Accessor accessor;
        accessor.bufferView = model.bufferViews.size();
        accessor.byteOffset = 0;
        accessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
        accessor.count = elementCount;
        accessor.type = type;

        model.bufferViews.push_back(view);
        model.accessors.push_back(accessor);

        return (int)model.bufferViews.size()-1;
    }
    int WorldExporter::SetPositionsForModel(size_t offsetInBuffer, tinygltf::Model& model, std::vector<float>& positions)
    {
        tinygltf::BufferView view;
        view.buffer = 0;
        view.byteOffset = offsetInBuffer;
        view.byteLength = positions.size() * sizeof(float);
        view.target = TINYGLTF_TARGET_ARRAY_BUFFER;

        tinygltf::Accessor accessor;
        accessor.bufferView = model.bufferViews.size();
        accessor.byteOffset = 0;
        accessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
        accessor.count = positions.size() / 3;
        accessor.type = TINYGLTF_TYPE_VEC3;

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
        accessor.minValues = { minX, minY, minZ};
        accessor.maxValues = { maxX, maxY, maxZ};

        model.bufferViews.push_back(view);
        model.accessors.push_back(accessor);

        return (int)model.bufferViews.size()-1;
    }

    int WorldExporter::SetIndicesForModel(size_t offsetInBuffer, tinygltf::Model& model, std::vector<unsigned int>& indices)
    {
        tinygltf::BufferView view;
        view.buffer = 0;
        view.byteOffset = offsetInBuffer;
        view.byteLength = indices.size() * sizeof(unsigned int);
        view.target = TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER;

        tinygltf::Accessor accessor;
        accessor.bufferView = model.bufferViews.size();
        accessor.byteOffset = 0;
        accessor.componentType = TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT;
        accessor.count = indices.size();
        accessor.type = TINYGLTF_TYPE_SCALAR;

        model.bufferViews.push_back(view);
        model.accessors.push_back(accessor);

        return (int)model.bufferViews.size()-1;
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
    std::vector<float> WorldExporter::BuildColorBuffer(const std::vector<Vertex>& vertices)
    {
        std::vector<float> colors;
        colors.reserve(vertices.size()*4);
        for (Vertex vertex : vertices)
        {
            colors.push_back((float)vertex.m_color.x);
            colors.push_back((float)vertex.m_color.y);
            colors.push_back((float)vertex.m_color.z);
            colors.push_back((float)vertex.m_color.w);
        }
        return colors;
    }
    std::vector<float> WorldExporter::BuildNormalBuffer(const std::vector<Vertex>& vertices)
    {
        std::vector<float> normals;
        normals.reserve(vertices.size()*3);
        for (Vertex vertex : vertices)
        {
            normals.push_back((float)vertex.m_normal.x);
            normals.push_back((float)vertex.m_normal.y);
            normals.push_back((float)vertex.m_normal.z);
        }
        return normals;
    }
    std::vector<float> WorldExporter::BuildUVBuffer(const std::vector<Vertex>& vertices)
    {
        std::vector<float> UVs;
        UVs.reserve(vertices.size()*2);
        for (Vertex vertex : vertices)
        {
            UVs.push_back((float)vertex.m_uv.x);
            UVs.push_back((float)vertex.m_uv.y);
        }
        return UVs;
    }
}
