#include "ResourceManager.h"
#include "FileFunctions.h"
#include "GPUResourceManager.h"
#include "RenderUnit.h"
#include "Renderer.h"
#include "Material.h"
#include "Texture2D.h"

#include <memory>
#include <set>

namespace WorldMaker
{
    std::unordered_map<unsigned int, MaterialWPtr> ResourceManager::s_materialCache = {};
    std::unordered_map<std::string, TextureWPtr> ResourceManager::s_textureCache;
    bool ResourceManager::s_inited = false;
    bool ResourceManager::s_ended = false;

    void ResourceManager::Init()
    {
        if (s_inited)
        {
            std::cerr << "Error: Can't init resource manager because it has already been initialized!\n";
            return;
        }
        CreateMaterial(diffuseTexDefaultPath, specularTexDefaultPath);
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
        s_materialCache.clear();
        s_textureCache.clear();
        s_ended = true;
    }

    // Texture handling ---------------------------------------------------------------------------------

    // Returns nullptr if the texture is not found
    TextureSPtr ResourceManager::GetTexture(const std::string &relativePath)
    {
        auto it = s_textureCache.find(relativePath);
        if (it != s_textureCache.end()) return it->second.lock();
        return nullptr;
    }

    // Returns nullptr if the texture is not found
    TextureSPtr ResourceManager::GetTexture(const std::vector<std::string>& relativePaths)
    {
        std::string unifiedPath = UnifyPaths(relativePaths);
        auto it = s_textureCache.find(unifiedPath);
        if (it != s_textureCache.end()) return it->second.lock();
        return nullptr;
    }

    void ResourceManager::RemoveTextureIfExpired(const std::string& relativePath)
    {
        auto it = s_textureCache.find(relativePath);

        if (it != s_textureCache.end() && it->second.expired())
        {
            s_textureCache.erase(relativePath);
            std::cout << "Texture of relative path " << relativePath << " destroyed\n";
        }
    }
    void ResourceManager::RemoveTextureIfExpired(const std::vector<std::string>& relativePaths)
    {
        std::string relativePath = UnifyPaths(relativePaths);
        auto it = s_textureCache.find(relativePath);

        if (it != s_textureCache.end() && it->second.expired())
        {
            s_textureCache.erase(relativePath);
            std::cout << "Texture of relative path " << relativePath << " destroyed\n";
        }
    }

    // Material handling ---------------------------------------------------------------------------------

    MaterialSPtr ResourceManager::CreateMaterial(const std::string& diffuseTexPath, const std::string& specularTexPath)
    {
        MaterialSPtr newMaterial = std::make_shared<Material>();
        newMaterial->m_diffuseTexture = LoadTexture<Texture2D>(diffuseTexPath);
        newMaterial->m_specularTexture = LoadTexture<Texture2D>(specularTexPath);

        s_materialCache[newMaterial->id()] = newMaterial;
        GPUResourceManager::CreateMaterial(newMaterial);
		return newMaterial;
    }
    MaterialSPtr ResourceManager::GetMaterial(unsigned int materialId)
    {
        auto it = s_materialCache.find(materialId);
        if (it != s_materialCache.end())
        {
            if (!it->second.expired()) return it->second.lock();
        }
        return nullptr;
    }
    void ResourceManager::RemoveMaterialIfExpired(unsigned int materialId)
    {
        auto it = s_materialCache.find(materialId);

        if (it != s_materialCache.end() && it->second.expired())
        {
            s_materialCache.erase(materialId);
            GPUResourceManager::ResetMaterial(materialId);
            std::cout << "Material of id " << materialId << " destroyed\n";
        }
    }
}
