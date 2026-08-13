#pragma once
#include "Texture2D.h"
#include "Vec4.h"
namespace WorldMaker
{
    class ModelMaterial
    {
    public:
        ModelMaterial(Texture2DSPtr diffuseTex, Texture2DSPtr specularTex);
        ~ModelMaterial();
        float m_shininess = 1.0f;

        Texture2DSPtr m_diffuseTexture = nullptr;
        Texture2DSPtr m_specularTexture = nullptr;

	    unsigned int id() const { return m_id; } // Is the instance id
    private:
        unsigned int m_id;
		static unsigned int s_idCount;
    };
    using ModelMaterialUPtr = std::unique_ptr<ModelMaterial>;
    using ModelMaterialSPtr = std::shared_ptr<ModelMaterial>;
    using ModelMaterialWPtr = std::weak_ptr<ModelMaterial>;
}
