#pragma once
#include "Material.h"
#include "Texture.h"
#include "Vec4.h"
namespace WorldMaker
{
    class Material
    {
    public:

        Material();
        Vec4 baseColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f); // blanco por defecto
        float m_mainTextureIndex = 0.0f;
        float m_diffuseIndex = 0.0f;
        float m_specularIndex = 0.0f;
        float m_reflectionIndex = 0.0f;
        float m_shininess = 1.0f;
        float m_cubemap = 0.0f;

        Texture* m_diffuseTexture = nullptr;
        Texture* m_specularTexture = nullptr;
        Texture* m_reflectionTexture = nullptr;

	    unsigned int id() const { return m_id; } // Is the instance id
    private:
        unsigned int m_id;
		static unsigned int s_idCount;
    };
    using MaterialUPtr = std::unique_ptr<Material>;
    using MaterialSPtr = std::shared_ptr<Material>;
    using MaterialWPtr = std::weak_ptr<Material>;
}
