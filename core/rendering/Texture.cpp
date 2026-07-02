#include "Texture.h"
#include "ResourceManager.h"

namespace WorldMaker
{
    Texture::~Texture() { ResourceManager::RemoveTextureIfExpired(m_path); }
}
