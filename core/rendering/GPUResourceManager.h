// Is a resource manager but for OpenGL stuff
#pragma once

#include "ArrayTexture2D.h"
#include "GPUTerrainMaterial.h"
#include "TerrainMaterial.h"
#include "Texture2DArray.h"
#include "SSBO.h"

#include <vector>

namespace WorldMaker
{
    class GPUResourceManager
    {
        static std::vector<GPUTerrainMaterialUPtr> s_materials;

        static Texture2DArrayUPtr s_texture2DArray;

        static SSBOUPtr<GPUTerrainMaterial> s_materialsSSBO;
    public:
        static void Init();
        static void Shutdown();
        static void PrepareToDraw();
        inline static Texture2DArray* GetTexture2DArray() { return s_texture2DArray.get();};
        static void CreateTerrainMaterial(TerrainMaterialSPtr mat);
        static void ResetMaterial(unsigned int matIndex, ArrayTexture2DSPtr diffuseTex, ArrayTexture2DSPtr specularTex);
        static bool s_inited;
        static bool s_ended;
    };
}
