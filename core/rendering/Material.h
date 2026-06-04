#pragma once
#include "Material.h"
#include "Texture2D.h"
#include "Vec4.h"
namespace WorldMaker
{
    class Material
    {
    public:

        Material();
        Vec4 m_baseColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f); // blanco por defecto
        float m_shininess = 1.0f;

        Texture2DWPtr m_diffuseTexture = {};
        Texture2DWPtr m_specularTexture = {};
        Texture2DWPtr m_reflectionTexture = {};

	    unsigned int id() const { return m_id; } // Is the instance id
    private:
        unsigned int m_id;
		static unsigned int s_idCount;
    };
    using MaterialSPtr = std::shared_ptr<Material>;
    using MaterialWPtr = std::weak_ptr<Material>;
}
