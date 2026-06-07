#include "Texture2DArray.h"
#include "DebugUtils.h"
#include "OpenGLUtils.h"
namespace WorldMaker
{
    Texture2DArray::Texture2DArray(int width, int height, int layers)
    {
        GLCall(glGenTextures(1, &m_glName));
        bind();
        GLCall(glTextureStorage3D(
           m_glName, 1, GL_RGBA8, width, height, layers
        ));
    }
    unsigned int Texture2DArray::pushTexture(Texture2D* texture)
    {
        unsigned int layer;
        if (!m_freeLayers.empty())
        {
            layer = m_freeLayers.front();
            m_freeLayers.pop_back();
        }
        else layer=m_nextLayer++;

        std::cout << "GL name: " << m_glName << "\n";
        std::cout << "Width: " << texture->width() << "\n";
        std::cout << "Height: " << texture->height() << "\n";
        std::cout << "Layer: " << layer << "\n";
        // std::cout << "Buffer size: " << texture->m_unsignedCharLocalBuffer << "\n";

        GLCall(glTextureSubImage3D(
            m_glName,
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
    void Texture2DArray::bind()
    {
        GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_glName));
    }
}
