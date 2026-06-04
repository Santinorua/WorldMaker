#include "Material.h"
#include "ResourceManager.h"
#include "Texture2D.h"

namespace WorldMaker
{
    unsigned int Material::s_idCount = 0;
    Material::Material() : m_id(s_idCount++) {};
}
