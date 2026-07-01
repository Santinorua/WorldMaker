#include "GPUResourceManager.h"
#include "GPUMaterial.h"
#include "RenderingConstants.h"
#include "ResourceManager.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Texture2DArray.h"
#include <memory>

namespace WorldMaker
{
    std::vector<GPUMaterialUPtr> GPUResourceManager::s_materials = {};
    Texture2DArrayUPtr GPUResourceManager::s_texture2DArray = nullptr;
    SSBOUPtr<GPUMaterial> GPUResourceManager::s_materialsSSBO = nullptr;
    bool GPUResourceManager::s_inited = false;
    bool GPUResourceManager::s_ended = false;
    void GPUResourceManager::CreateMaterial(Material* mat)
    {
        s_materials.push_back(std::move(std::make_unique<GPUMaterial>(mat)));
        GPUMaterial* newMat = s_materials.back().get();
        if (s_materialsSSBO.get()->checkIfEnoughSpaceForPush(1))
        {
            s_materialsSSBO.get()->pushData(*newMat);
            unsigned int lastElement = s_materialsSSBO.get()->currentElements()-1;
            s_materialsSSBO.get()->submitSubData(lastElement, lastElement);
        }
    }
    void GPUResourceManager::DestroyMaterial(unsigned int matIndex)
    {
        s_materials[matIndex]->reset();
        s_materialsSSBO.get()->submitSubData(matIndex, matIndex);
    }
    void GPUResourceManager::Init()
    {
        if (s_inited)
        {
            std::cerr << "Error: Can't init GPUResourceManager because it has already been initialized\n";
            return;
        }
        s_texture2DArray = std::make_unique<Texture2DArray>(terrainTexturesWidth, terrainTexturesHeight, maxTextures);
        s_materialsSSBO = std::move(std::make_unique<SSBO<GPUMaterial>>(maxMaterials,GL_DYNAMIC_STORAGE_BIT));
        s_inited = true;
    }
    void  GPUResourceManager::Shutdown()
    {
        if (s_ended)
        {
            std::cerr << "Error: Can't end gpu resource manager because it has already been ended!\n";
            return;
        }
        s_texture2DArray.reset();
        s_materials.clear();
        s_materialsSSBO->flush();
        s_ended = true;
    }
    void GPUResourceManager::PrepareToDraw()
    {
        s_materialsSSBO->bindBufferBase(SSBOType::materials);
        ShaderProgram::s_boundShader->loadTexture2DArray(s_texture2DArray.get()->glName());
    }
}
