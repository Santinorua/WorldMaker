#include "Material.h"
#include "ResourceManager.h"
#include "Texture2D.h"

namespace WorldMaker
{
    unsigned int Material::s_idCount = 0;
    Material::Material() : m_id(s_idCount++) {};
    Material::~Material()
    {
        ResourceManager::RemoveTextureIfExpired(m_diffuseTexture->path());
        ResourceManager::RemoveTextureIfExpired(m_specularTexture->path());
        ResourceManager::RemoveMaterialIfExpired(m_id);
    }
    void Material::reset()
    {
        m_shininess = 1;
        ResourceManager::RemoveTextureIfExpired(m_diffuseTexture->path());
        ResourceManager::RemoveTextureIfExpired(m_specularTexture->path());
        m_diffuseTexture = ResourceManager::LoadTexture<Texture2D>(diffuseTexEmptyPath);
        m_specularTexture = ResourceManager::LoadTexture<Texture2D>(specularTexDefaultPath);
    }
}
