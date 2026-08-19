#include "MeshMaterial.h"
#include "ResourceManager.h"

namespace WorldMaker
{
    unsigned int MeshMaterial::s_idCount = 0;
    MeshMaterial::MeshMaterial(Texture2DSPtr diffuseTex, Texture2DSPtr specularTex)
    : m_id(s_idCount++), m_diffuseTexture(diffuseTex), m_specularTexture(specularTex)
    {

    }
    MeshMaterial::~MeshMaterial()
    {
        ResourceManager::RemoveMeshMaterialIfExpired(m_id);
    }
}
