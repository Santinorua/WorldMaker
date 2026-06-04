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
    std::unordered_map<unsigned int, ResourceManager::MaterialEntry> ResourceManager::m_materialCache;
    std::unordered_map<std::string, ResourceManager::TextureEntry> ResourceManager::m_textureCache;

    void ResourceManager::Init()
    {

    }

    // Texture handling ---------------------------------------------------------------------------------

    // Returns nullptr if the texture is not found
    TextureWPtr ResourceManager::GetTexture(const std::string &relativePath)
    {
        auto it = m_textureCache.find(relativePath);
        if (it != m_textureCache.end()) return it->second.texture;
        return {};
    }

    // Returns nullptr if the texture is not found
    TextureWPtr ResourceManager::GetTexture(const std::vector<std::string>& relativePaths)
    {
        std::string unifiedPath = UnifyPaths(relativePaths);
        auto it = m_textureCache.find(unifiedPath);
        if (it != m_textureCache.end()) return it->second.texture;
        return {};
    }

    // Material handling ---------------------------------------------------------------------------------

    MaterialWPtr ResourceManager::CreateMaterial(const std::string& diffuseTexPath, const std::string& specularTexPath, const std::string& reflectionTexPath)
    {
   		MaterialSPtr newMat = std::make_shared<Material>();
        newMat->m_diffuseTexture = std::static_cast<Texture2DWPtr>(LoadTexture<Texture2D>(diffuseTexPath));
        newMat->m_specularTexture = LoadTexture<Texture2D>(specularTexPath);

        GPUResourceManager::CreateMaterial(newMat.get())

		s_materialCache.push_back(newMat);
		return newMat;
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
    MaterialWPtr ResourceManager::GetMaterial(unsigned int materialId)
    {
   		auto it = m_materialCache.find(materialId);
    	if (it == m_materialCache.end())
		{
			std::cout << "Warning: There is not such a material with id " << materialId << " to destroy\n";
           return {};
		}
     	it->second.refCount++;
    	return it->second.material;
    }
}
