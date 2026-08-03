#include "ResourceManager.h"
#include "ArrayTexture2D.h"
#include "FileFunctions.h"
#include "GPUResourceManager.h"
#include "MeshMaterial.h"
#include "Pointers.h"
#include "RenderUnit.h"
#include "Renderer.h"
#include "ShaderProgram.h"
#include "TerrainMaterial.h"
#include "Texture2D.h"

#include <memory>
#include <set>

namespace WorldMaker
{
    std::unordered_map<unsigned int, TerrainMaterialWPtr> ResourceManager::s_terrainMaterialCache = {};
    std::unordered_map<unsigned int, MeshMaterialWPtr> ResourceManager::s_MeshMaterialCache = {};
    std::unordered_map<std::string, ArrayTexture2DWPtr> ResourceManager::s_arrayTexture2DCache = {};
    std::unordered_map<std::string, Texture2DWPtr> ResourceManager::s_texture2DCache = {};
    std::unordered_map<std::string, ModelWPtr> ResourceManager::s_modelCache;
    std::vector<TerrainMaterialSPtr> ResourceManager::s_terrainMaterials = {};
    bool ResourceManager::s_inited = false;
    bool ResourceManager::s_ended = false;

    void ResourceManager::Init()
    {
        if (s_inited)
        {
            std::cerr << "Error: Can't init resource manager because it has already been initialized!\n";
            return;
        }
        // CreateTerrainMaterial(diffuseTexDefaultGrassPath, specularTexDefaultPath);
        s_terrainMaterials.push_back(CreateTerrainMaterial(diffuseTexDefaultGrassPath, specularTexDefaultPath));
        s_terrainMaterials.push_back(CreateTerrainMaterial(diffuseTexDefaultGrassPath, specularTexDefaultPath));
        s_inited = true;
    }

    void ResourceManager::Shutdown()
    {
        std::cout << "shutdown called\n";
        if (s_ended)
        {
            std::cerr << "Error: Can't end resource manager because it has already been ended!\n";
            return;
        }
        s_terrainMaterialCache.clear();
        s_arrayTexture2DCache.clear();
        s_texture2DCache.clear();
        s_ended = true;
    }

    // Texture handling ---------------------------------------------------------------------------------

    Texture2DSPtr ResourceManager::LoadTexture(const std::string& relativePath)
    {
        auto it = s_texture2DCache.find(relativePath);
        if (it != s_texture2DCache.end())
        {
            if (!it->second.expired()) return GetShared<Texture2D>(it->second);
            else s_texture2DCache.erase(it);
        }

        Texture2DSPtr newTexture = std::make_shared<Texture2D>(relativePath);

        s_texture2DCache[relativePath] = newTexture;
        return newTexture;
    }
    Texture2DSPtr ResourceManager::LoadTexture(const std::string& modelTexPath, const aiTexture* tex)
    {
        auto it = s_texture2DCache.find(modelTexPath);
        if (it != s_texture2DCache.end())
        {
            if (!it->second.expired()) return GetShared<Texture2D>(it->second);
            else s_texture2DCache.erase(it);
        }

        Texture2DSPtr newTexture = std::make_shared<Texture2D>(modelTexPath, tex);

        s_texture2DCache[modelTexPath] = newTexture;
        return newTexture;
    }
    ArrayTexture2DSPtr ResourceManager::LoadArrayTexture(const std::string& relativePath)
    {
        auto it = s_arrayTexture2DCache.find(relativePath);
        if (it != s_arrayTexture2DCache.end())
        {
            if (!it->second.expired()) return GetShared<ArrayTexture2D>(it->second);
            else s_arrayTexture2DCache.erase(it);
        }

        ArrayTexture2DSPtr newTexture = std::make_shared<ArrayTexture2D>(relativePath);

        s_arrayTexture2DCache[relativePath] = newTexture;
        return newTexture;
    }

    // Returns nullptr if the texture is not found
    TextureSPtr ResourceManager::GetTexture2D(const std::string &relativePath)
    {
        auto it = s_texture2DCache.find(relativePath);
        if (it != s_texture2DCache.end()) return it->second.lock();
        return nullptr;
    }
    ArrayTexture2DSPtr ResourceManager::GetArrayTexture2D(const std::string& relativePath)
    {
        auto it = s_arrayTexture2DCache.find(relativePath);
        if (it != s_arrayTexture2DCache.end()) return it->second.lock();
        return nullptr;
    }
    // Returns nullptr if the texture is not found
    // TextureSPtr ResourceManager::GetTexture(const std::vector<std::string>& relativePaths)
    // {
    //     std::string unifiedPath = unifyPaths(relativePaths);
    //     auto it = s_arrayTexture2DCache.find(unifiedPath);
    //     if (it != s_arrayTexture2DCache.end()) return it->second.lock();
    //     return nullptr;
    // }

    void ResourceManager::RemoveArrayTexture2DIfExpired(const std::string& relativePath, ArrayTexture2D* tex)
    {
        auto it = s_arrayTexture2DCache.find(relativePath);

        if (it != s_arrayTexture2DCache.end() && it->second.expired())
        {
            GPUResourceManager::GetTexture2DArray()->resetTextureLayer(tex);
            s_arrayTexture2DCache.erase(relativePath);
            std::cout << "ArrayTexture2D of relative path " << relativePath << " destroyed\n";
        }
    }
    void ResourceManager::RemoveTexture2DIfExpired(const std::string& relativePath)
    {
        auto it = s_texture2DCache.find(relativePath);

        if (it != s_texture2DCache.end() && it->second.expired())
        {
            s_texture2DCache.erase(relativePath);
            std::cout << "Texture2D of relative path " << relativePath << " destroyed\n";
        }
    }

    // Material handling ---------------------------------------------------------------------------------

    // Take into account that textures 2D are created and pushed into the TextureArray, so they will still "exist" in spite of being deleted
    TerrainMaterialSPtr ResourceManager::CreateTerrainMaterial(const std::string& diffuseTexPath, const std::string& specularTexPath)
    {
        ArrayTexture2DSPtr diffuseTex = LoadArrayTexture(diffuseTexPath);
        ArrayTexture2DSPtr specularTex = LoadArrayTexture(specularTexPath);
        TerrainMaterialSPtr newMat = std::make_shared<TerrainMaterial>(diffuseTex, specularTex);
        GPUResourceManager::CreateTerrainMaterial(newMat);
        s_terrainMaterialCache[newMat->id()] = newMat;
		return newMat;
    }
    TerrainMaterialSPtr ResourceManager::GetTerrainMaterial(unsigned int materialId)
    {
        auto it = s_terrainMaterialCache.find(materialId);
        if (it != s_terrainMaterialCache.end())
        {
            if (!it->second.expired()) return it->second.lock();
        }
        return nullptr;
    }
    MeshMaterialSPtr ResourceManager::GetMeshMaterial(unsigned int materialId)
    {
        auto it = s_MeshMaterialCache.find(materialId);
        if (it != s_MeshMaterialCache.end())
        {
            if (!it->second.expired()) return GetShared<MeshMaterial>(it->second);
        }
        return nullptr;
    }
    void ResourceManager::RemoveTerrainMaterialIfExpired(unsigned int materialId)
    {
        auto it = s_terrainMaterialCache.find(materialId);

        if (it != s_terrainMaterialCache.end() && it->second.expired())
        {
            s_terrainMaterialCache.erase(materialId);
            ArrayTexture2DSPtr emptyTex = LoadArrayTexture(diffuseTexEmptyPath);
            ArrayTexture2DSPtr specularTex = LoadArrayTexture(specularTexDefaultPath);
            GPUResourceManager::ResetMaterial(materialId, emptyTex, specularTex);
            std::cout << "Terrain Material of id " << materialId << " destroyed\n";
        }
    }

    MeshMaterialSPtr ResourceManager::CreateMeshMaterial(Texture2DSPtr diffuse, Texture2DSPtr specular)
    {
        MeshMaterialSPtr newMat = std::make_shared<MeshMaterial>(diffuse, specular);
        s_MeshMaterialCache[newMat->id()] = newMat;
        return newMat;
    }

    void ResourceManager::RemoveMeshMaterialIfExpired(unsigned int materialId)
    {
        auto it = s_MeshMaterialCache.find(materialId);

        if (it != s_MeshMaterialCache.end() && it->second.expired())
        {
            s_MeshMaterialCache.erase(materialId);
            std::cout << "Model Material of id " << materialId << " destroyed\n";
        }
    }
    // Model handling ---------------------------------------------------------------------------------

    ModelSPtr ResourceManager::LoadModel(const std::string& relativePath)
    {
        auto it = s_modelCache.find(relativePath);
        if (it != s_modelCache.end())
        {
            if (!it->second.expired()) return it->second.lock();
        }

        ModelSPtr newModel = std::make_shared<Model>(relativePath);

        s_modelCache[relativePath] = newModel;

        return newModel;
    }
    ModelSPtr ResourceManager::GetModel(const std::string& relativePath)
    {
	   	auto it = s_modelCache.find(relativePath);
		if (it != s_modelCache.end())
	    {
			if (!it->second.expired()) return it->second.lock();
	    }
		return nullptr;
    }
    void ResourceManager::RemoveModelIfExpired(const std::string& relativePath)
    {
        auto it = s_modelCache.find(relativePath);

        if (it != s_modelCache.end() && it->second.expired())
        {
            s_modelCache.erase(relativePath);
            std::cout << "Model of relative " << relativePath << " destroyed\n";
        }
    }
}
