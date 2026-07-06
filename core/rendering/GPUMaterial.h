#pragma once

#include "Vec4.h"
#include "Material.h"

namespace WorldMaker
{
    struct GPUMaterial
    {
        GPUMaterial(MaterialSPtr mat);
        void reset();
        float m_diffuseIndex = 0.0f;
        float m_specularIndex = 0.0f;
        float m_shininess = 1.0f;
    };
    using GPUMaterialSPtr = std::shared_ptr<GPUMaterial>;
    using GPUMaterialUPtr = std::unique_ptr<GPUMaterial>;
}
