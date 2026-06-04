#include "ResourceManager.h"
#include "FileFunctions.h"
#include "RenderUnit.h"
#include "Renderer.h"
#include "Material.h"
#include "Texture2D.h"

#include <memory>
#include <set>

namespace WorldMaker
{
    std::unordered_map<unsigned int, ResourceManager::MaterialEntry> ResourceManager::m_materialCache;
    std::unordered_map<std::string, ResourceManager::TextureEntry> ResourceManager::m_textureCache;

    void ResourceManager::Init()
    {

    }

    // Texture handling ---------------------------------------------------------------------------------

    // Returns nullptr if the texture is not found
    Texture* ResourceManager::GetTexture(const std::string &relativePath)
    {
        auto it = m_textureCache.find(relativePath);
        if (it != m_textureCache.end()) return it->second.texture.get();
        return {};
    }

    // Returns nullptr if the texture is not found
    Texture* ResourceManager::GetTexture(const std::vector<std::string>& relativePaths)
    {
        std::string unifiedPath = UnifyPaths(relativePaths);
        auto it = m_textureCache.find(unifiedPath);
        if (it != m_textureCache.end()) return it->second.texture.get();
        return {};
    }

    // Material handling ---------------------------------------------------------------------------------

    Material* ResourceManager::CreateMaterial(const std::string& diffuseTexPath, const std::string& specularTexPath, const std::string& reflectionTexPath)
    {
   		MaterialUPtr newMaterial = std::make_unique<Material>();
		unsigned int id = newMaterial->id();
        newMaterial->m_diffuseTexture = LoadTexture<Texture2D>(diffuseTexPath);
        newMaterial->m_specularTexture = LoadTexture<Texture2D>(specularTexPath);
        newMaterial->m_reflectionTexture = LoadTexture<Texture2D>(reflectionTexPath);

		MaterialEntry entry = {.material = std::move(newMaterial), .refCount = 1};
		m_materialCache.emplace(id, std::move(entry));
		return m_materialCache[id].material.get();
    }
    void ResourceManager::DestroyMaterial(unsigned int materialId)
    {
    	auto it = m_materialCache.find(materialId);
     	if (it == m_materialCache.end())
		{
			std::cout << "Warning: There is not such a material with id " << materialId << " to destroy\n";
            return;
		}
      	it->second.refCount--;

       	if (it->second.refCount<=0)
        {
       		m_materialCache.erase(materialId);
        }

    }
    Material* ResourceManager::GetMaterial(unsigned int materialId)
    {
   		auto it = m_materialCache.find(materialId);
    	if (it == m_materialCache.end())
		{
			std::cout << "Warning: There is not such a material with id " << materialId << " to destroy\n";
			return nullptr;
		}
     	it->second.refCount++;
    	return it->second.material.get();
    }
}
