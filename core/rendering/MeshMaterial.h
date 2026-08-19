#pragma once
#include "Texture2D.h"
#include "Vec4.h"
namespace WorldMaker
{
    class MeshMaterial
    {
    public:
        MeshMaterial(Texture2DSPtr diffuseTex, Texture2DSPtr specularTex);
        ~MeshMaterial();
        float m_shininess = 1.0f;

        Texture2DSPtr m_diffuseTexture = nullptr;
        Texture2DSPtr m_specularTexture = nullptr;

	    unsigned int id() const { return m_id; } // Is the instance id
    private:
        unsigned int m_id;
		static unsigned int s_idCount;
    };
    using MeshMaterialUPtr = std::unique_ptr<MeshMaterial>;
    using MeshMaterialSPtr = std::shared_ptr<MeshMaterial>;
    using MeshMaterialWPtr = std::weak_ptr<MeshMaterial>;
}
