#pragma once

#include "Pointers.h"
#include "ArrayTexture2D.h"
#include "ModelMaterial.h"
#include "Model.h"
#include "TerrainMaterial.h"
#include "Texture2D.h"

#include <unordered_map>
#include <string>
#include <memory>

namespace WorldMaker
{
    class TerrainMaterial;

    class ResourceManager
    {
    public:
        static void Init();
        static void Shutdown();

        // Textures handling ---------------------------------------------------------------------------------
        static Texture2DSPtr LoadTexture(const std::string& relativePath);
        static Texture2DSPtr LoadTexture(const std::string& modelTexPath, const aiTexture* tex);
        static ArrayTexture2DSPtr LoadArrayTexture(const std::string& relativePath);
        static TextureSPtr GetTexture2D(const std::string& relativePath);
        static ArrayTexture2DSPtr GetArrayTexture2D(const std::string& relativePath);
        // static TextureSPtr GetTexture(const std::vector<std::string>& relativePaths);
        static void RemoveArrayTexture2DIfExpired(const std::string& relativePath, ArrayTexture2D* tex);
        static void RemoveTexture2DIfExpired(const std::string& relativePath);
        // static void RemoveTextureIfExpired(const std::vector<std::string>& relativePaths);

        // Material handling ---------------------------------------------------------------------------------
        static TerrainMaterialSPtr CreateTerrainMaterial(const std::string& diffuseTexPath, const std::string& specularTexPath = diffuseTexDefaultPath);
        static TerrainMaterialSPtr GetTerrainMaterial(unsigned int materialId);
        static void RemoveTerrainMaterialIfExpired(unsigned int materialId);
        static ModelMaterialSPtr CreateModelMaterial(Texture2DSPtr diffuse, Texture2DSPtr specular);
        static ModelMaterialSPtr GetModelMaterial(unsigned int materialId);
        static void RemoveModelMaterialIfExpired(unsigned int materialId);

        // Model handling ---------------------------------------------------------------------------------
        static ModelSPtr LoadModel(const std::string& relativePath);
        static ModelSPtr GetModel(const std::string& relativePath);
        static void RemoveModelIfExpired(const std::string& relativePath);

    private:

        static bool s_inited;
        static bool s_ended;
public:
        static std::vector<TerrainMaterialSPtr> s_terrainMaterials; // SHOULD BE HANDLED BY THE UI AND EDITOR
        static std::unordered_map<std::string, ArrayTexture2DWPtr> s_arrayTexture2DCache;
        static std::unordered_map<std::string, Texture2DWPtr> s_texture2DCache;
        static std::unordered_map<unsigned int, TerrainMaterialWPtr> s_terrainMaterialCache;
        static std::unordered_map<unsigned int, ModelMaterialWPtr> s_modelMaterialCache;
        static std::unordered_map<std::string, ModelWPtr> s_modelCache;
    };
}
