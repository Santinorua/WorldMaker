#pragma once
#include "Texture2D.h"
#include "Vec4.h"
namespace WorldMaker
{
    class Material
    {
    public:

        Material();
        ~Material();
        void reset();
        float m_shininess = 1.0f;

        Texture2D* m_diffuseTexture = nullptr;
        Texture2D* m_specularTexture = nullptr;

	    unsigned int id() const { return m_id; } // Is the instance id
    private:
        unsigned int m_id;
		static unsigned int s_idCount;
    };
    using MaterialUPtr = std::unique_ptr<Material>;
    using MaterialSPtr = std::shared_ptr<Material>;
    using MaterialWPtr = std::weak_ptr<Material>;
}
