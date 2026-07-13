#pragma once
#include "ArrayTexture2D.h"
#include "RenderingAssets.h"
#include "Vec4.h"
namespace WorldMaker
{
    class TerrainMaterial
    {
    public:
        TerrainMaterial(ArrayTexture2DSPtr diffuseTex, ArrayTexture2DSPtr specularTex);
        ~TerrainMaterial();
        void reset(const std::string& diffusePath = diffuseTexEmptyPath, const std::string& specularPath = diffuseTexDefaultPath);
        float m_shininess = 1.0f;

        ArrayTexture2DSPtr m_diffuseTexture = nullptr;
        ArrayTexture2DSPtr m_specularTexture = nullptr;

	    unsigned int id() const { return m_id; } // Is the instance id
    private:
        unsigned int m_id;
		static unsigned int s_idCount;
    };
    using TerrainMaterialUPtr = std::unique_ptr<TerrainMaterial>;
    using TerrainMaterialSPtr = std::shared_ptr<TerrainMaterial>;
    using TerrainMaterialWPtr = std::weak_ptr<TerrainMaterial>;
}
