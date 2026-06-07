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
    std::vector<ResourceManager::MaterialEntry> ResourceManager::s_materialCache = {};
    std::unordered_map<std::string, ResourceManager::TextureEntry> ResourceManager::s_textureCache;
    bool ResourceManager::s_inited = false;

    void ResourceManager::Init()
    {
        if (s_inited) std::cerr << "Error: Can't init resource manager because it has already been initialized!\n";
        CreateMaterial(diffuseTexDefaultPath, specularTexDefaultPath);
        CreateMaterial(diffuseTexDefaultPath, specularTexDefaultPath);
        CreateMaterial(diffuseTexDefaultPath, specularTexDefaultPath);
        s_inited = true;
    }

    // Texture handling ---------------------------------------------------------------------------------

    // Returns nullptr if the texture is not found
    Texture* ResourceManager::GetTexture(const std::string &relativePath)
    {
        auto it = s_textureCache.find(relativePath);
        if (it != s_textureCache.end()) return it->second.texture.get();
        return nullptr;
    }

    // Returns nullptr if the texture is not found
    Texture* ResourceManager::GetTexture(const std::vector<std::string>& relativePaths)
    {
        std::string unifiedPath = UnifyPaths(relativePaths);
        auto it = s_textureCache.find(unifiedPath);
        if (it != s_textureCache.end()) return it->second.texture.get();
        return {};
    }

    // Material handling ---------------------------------------------------------------------------------

    Material* ResourceManager::CreateMaterial(const std::string& diffuseTexPath, const std::string& specularTexPath)
    {
        MaterialEntry newEntry;
        newEntry.refCount = 1;
        newEntry.material = std::make_unique<Material>();
        s_materialCache.push_back(std::move(newEntry));
		Material* mat = s_materialCache.front().material.get();
        mat->m_diffuseTexture = static_cast<Texture2D*>(LoadTexture<Texture2D>(diffuseTexPath));
        mat->m_specularTexture = static_cast<Texture2D*>(LoadTexture<Texture2D>(specularTexPath));
		GPUResourceManager::CreateMaterial(mat);
		return s_materialCache.front().material.get();
    }
    void ResourceManager::DestroyMaterial(unsigned int materialIndex)
    {
        if (materialIndex>=s_materialCache.size())
        {
            std::cerr << "Error: Trying to delete a material with an index (" << materialIndex << ") out of the bounds of length in s_materialCache\n";
        }

       	s_materialCache[materialIndex].refCount--;

       	if (s_materialCache[materialIndex].refCount<=0)
        {
            s_materialCache[materialIndex].material.reset();
            GPUResourceManager::DestroyMaterial(materialIndex);
        }

    }
    Material* ResourceManager::GetMaterial(unsigned int materialIndex)
    {
    	return s_materialCache[materialIndex].material.get();
    }
}
