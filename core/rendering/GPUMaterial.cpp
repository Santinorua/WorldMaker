#include "GPUMaterial.h"
#include "Pointers.h"

namespace WorldMaker
{
    GPUMaterial::GPUMaterial(Material* mat)
    {
        m_diffuseIndex = GetShared(mat->m_diffuseTexture)->texture2DArrayLayer();
        m_specularIndex = GetShared(mat->m_specularTexture)->texture2DArrayLayer();
        m_shininess = mat->m_shininess;
    }
    void GPUMaterial::reset()
    {
        m_diffuseIndex = 0;
        m_specularIndex = 0;
        m_shininess = 1;
    }
}
