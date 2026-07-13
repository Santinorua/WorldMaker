#include "Texture2DArray.h"
#include "DebugUtils.h"
#include "OpenGLUtils.h"
#include "ResourceManager.h"
namespace WorldMaker
{
    Texture2DArray::Texture2DArray(int width, int height, int layers)
    {
        GLCall(glGenTextures(1, &m_glName));
        bind();
        GLCall(glTexStorage3D(
           GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, layers
        ));
    }
    unsigned int Texture2DArray::pushTexture(ArrayTexture2D* texture)
    {
        unsigned int layer;
        if (!m_freeLayers.empty())
        {
            layer = m_freeLayers.back();
            m_freeLayers.pop_back();
        }
        else layer=m_nextLayer++;

        GLCall(glTexSubImage3D(
            GL_TEXTURE_2D_ARRAY,
            0,
            0,0,layer,
            texture->width(),
            texture->height(),
            1,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            texture->m_unsignedCharLocalBuffer));

        return layer;
    }
    void Texture2DArray::resetTextureLayer(unsigned int layer)
    {
        Texture2DSPtr emptyTex = ResourceManager::LoadTexture(diffuseTexEmptyPath);
        ASSERT(layer < m_nextLayer);
        ASSERT(layer >= 0);
        GLCall(glTexSubImage3D(
            GL_TEXTURE_2D_ARRAY,
            0,
            0,0,layer,
            emptyTex->width(),
            emptyTex->height(),
            1,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            emptyTex->m_unsignedCharLocalBuffer));
        m_freeLayers.push_back(layer);
    }
    void Texture2DArray::bind()
    {
        GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_glName));
    }
}
