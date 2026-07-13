#include "Texture.h"
#include "ResourceManager.h"

namespace WorldMaker
{
    Texture::Texture() {}
    Texture::~Texture() { ResourceManager::RemoveTexture2DIfExpired(m_path); }
}
