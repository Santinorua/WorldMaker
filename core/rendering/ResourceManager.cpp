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
    std::vector<MaterialUPtr> ResourceManager::s_materialCache = {};
    std::unordered_map<std::string, ResourceManager::TextureEntry> ResourceManager::s_textureCache;

    void ResourceManager::Init()
    {

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

    Material* ResourceManager::CreateMaterial(const std::string& diffuseTexPath, const std::string& specularTexPath, const std::string& reflectionTexPath)
    {
		s_materialCache.push_back(std::make_unique<Material>());
		Material* mat = s_materialCache.front().get();
		GPUResourceManager::CreateMaterial(s_materialCache.front().get());
        mat->m_diffuseTexture = static_cast<Texture2D*>(LoadTexture<Texture2D>(diffuseTexPath));
        mat->m_specularTexture = static_cast<Texture2D*>(LoadTexture<Texture2D>(specularTexPath));
		return s_materialCache.front().get();
    }
    void ResourceManager::DestroyMaterial(unsigned int materialId)
    {
        // TODO
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
    Material* ResourceManager::GetMaterial(unsigned int materialIndex)
    {
    	return s_materialCache[materialIndex].get();
    }
}
