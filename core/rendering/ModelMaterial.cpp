#include "ModelMaterial.h"
#include "ResourceManager.h"

namespace WorldMaker
{
    unsigned int ModelMaterial::s_idCount = 0;
    ModelMaterial::ModelMaterial(Texture2DSPtr diffuseTex, Texture2DSPtr specularTex)
    : m_id(s_idCount++), m_diffuseTexture(diffuseTex), m_specularTexture(specularTex)
    {

    }
    ModelMaterial::~ModelMaterial()
    {
        ResourceManager::RemoveModelMaterialIfExpired(m_id);
    }
}
