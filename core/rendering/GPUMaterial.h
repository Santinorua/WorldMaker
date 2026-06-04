#pragma once

#include "Vec4.h"
#include "Material.h"

namespace WorldMaker
{
    struct GPUMaterial
    {

        GPUMaterial(Material* mat);
        Vec4 m_baseColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f); // blanco por defecto
        float m_diffuseIndex = 0.0f;
        float m_specularIndex = 0.0f;
        float m_shininess = 1.0f;
    };
    using GPUMaterialSPtr = std::shared_ptr<GPUMaterial>;
    using GPUMaterialUPtr = std::unique_ptr<GPUMaterial>;
}
