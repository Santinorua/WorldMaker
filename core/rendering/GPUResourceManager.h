// Is a resource manager but for OpenGL stuff
#pragma once

#include "GPUMaterial.h"
#include "Material.h"
#include "Texture2DArray.h"
#include "SSBO.h"

#include <vector>

namespace WorldMaker
{
    class GPUResourceManager
    {
        static std::vector<GPUMaterialUPtr> s_materials;

        static Texture2DArrayUPtr s_texture2DArray;

        static SSBOUPtr<GPUMaterial> s_materialsSSBO;
    public:
        static void Init();
        static void Shutdown();
        static void PrepareToDraw();
        inline static Texture2DArray* GetTexture2DArray() { return s_texture2DArray.get();};
        static void CreateMaterial(MaterialSPtr mat);
        static void ResetMaterial(unsigned int matIndex);
        static bool s_inited;
        static bool s_ended;
    };
}
