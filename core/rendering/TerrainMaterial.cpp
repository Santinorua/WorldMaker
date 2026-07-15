#include "TerrainMaterial.h"
#include "GPUResourceManager.h"
#include "ResourceManager.h"
#include "Texture2D.h"

namespace WorldMaker
{
    unsigned int TerrainMaterial::s_idCount = 0;
    TerrainMaterial::TerrainMaterial(ArrayTexture2DSPtr diffuseTex, ArrayTexture2DSPtr specularTex) :
    m_id(s_idCount++), m_diffuseTexture(diffuseTex), m_specularTexture(specularTex)
    {};
    TerrainMaterial::~TerrainMaterial()
    {
        ResourceManager::RemoveTerrainMaterialIfExpired(m_id);
    }
    void TerrainMaterial::reset(const std::string& diffusePath, const std::string& specularPath)
    {
        m_shininess = 1;
        m_diffuseTexture = ResourceManager::LoadArrayTexture(diffusePath);
        m_specularTexture = ResourceManager::LoadArrayTexture(specularPath);
        GPUResourceManager::ResetMaterial(m_id, m_diffuseTexture, m_specularTexture);
    }
}
