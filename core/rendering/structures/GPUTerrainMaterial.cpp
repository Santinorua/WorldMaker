#include "GPUTerrainMaterial.h"
#include "Pointers.h"
#include "TerrainMaterial.h"

namespace WorldMaker
{
    GPUTerrainMaterial::GPUTerrainMaterial(TerrainMaterialSPtr mat)
    {
        m_diffuseIndex = mat->m_diffuseTexture->texture2DArrayLayer();
        m_specularIndex = mat->m_specularTexture->texture2DArrayLayer();
        m_shininess = mat->m_shininess;
    }
    void GPUTerrainMaterial::reset(ArrayTexture2DSPtr diffuseTex, ArrayTexture2DSPtr specularTex)
    {
        m_diffuseIndex = diffuseTex->texture2DArrayLayer();
        m_specularIndex = specularTex->texture2DArrayLayer();
        m_shininess = 1;
    }
}
