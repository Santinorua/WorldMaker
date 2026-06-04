#include "GPUResourceManager.h"
#include "GPUMaterial.h"
#include <memory>

namespace WorldMaker
{
    void GPUResourceManager::CreateMaterial(Material* mat)
    {
        GPUMaterialUPtr newMat = std::make_unique<GPUMaterial>(mat);
        s_materials.push_back(std::move(newMat));
    }
}
