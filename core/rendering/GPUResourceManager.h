// Is a resource manager but for OpenGL stuff
#pragma once

#include "GPUMaterial.h"
#include "Material.h"
#include "Texture2DArray.h"

#include <vector>

namespace WorldMaker
{
    class GPUResourceManager
    {
        static std::vector<GPUMaterialUPtr> s_materials;

        static Texture2DArrayUPtr s_texture2DArray;

    public:
        inline static Texture2DArray* Texture2DArray() { return s_texture2DArray.get();};

        static void CreateMaterial(Material* mat);
    };
}
