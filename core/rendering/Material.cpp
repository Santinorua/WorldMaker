#include "Material.h"
#include "ResourceManager.h"
#include "Texture2D.h"

namespace WorldMaker
{
    unsigned int Material::s_idCount = 0;
    Material::Material() : m_id(s_idCount++) {};
    Material::~Material()
    {
        std::cout << "About to destroy material " <<"\n";
        ResourceManager::UnloadTexture(m_diffuseTexture->path());
        ResourceManager::UnloadTexture(m_specularTexture->path());
    }
    void Material::reset()
    {
        m_shininess = 1;
        ResourceManager::UnloadTexture(m_diffuseTexture->path());
        ResourceManager::UnloadTexture(m_specularTexture->path());
        m_diffuseTexture = static_cast<Texture2D*>(ResourceManager::LoadTexture<Texture2D>(diffuseTexEmptyPath));
        m_specularTexture = static_cast<Texture2D*>(ResourceManager::LoadTexture<Texture2D>(specularTexDefaultPath));
    }
}
