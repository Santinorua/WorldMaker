#pragma once

#include "TerrainMaterial.h"

namespace WorldMaker
{
    struct GPUTerrainMaterial
    {
        GPUTerrainMaterial(TerrainMaterialSPtr mat);
        void reset(ArrayTexture2DSPtr diffuseTex, ArrayTexture2DSPtr specularTex);
        float m_diffuseIndex = 0.0f;
        float m_specularIndex = 0.0f;
        float m_shininess = 1.0f;
    };
    using GPUTerrainMaterialSPtr = std::shared_ptr<GPUTerrainMaterial>;
    using GPUTerrainMaterialUPtr = std::unique_ptr<GPUTerrainMaterial>;
}
