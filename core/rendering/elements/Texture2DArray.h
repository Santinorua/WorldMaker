#pragma once
#include "OpenGLUtils.h"
#include "ArrayTexture2D.h"
#include <vector>

namespace WorldMaker
{
    class Texture2DArray
    {
    public:
        Texture2DArray(int width, int height, int layers);
        unsigned int pushTexture(ArrayTexture2D* texture);
        void resetTextureLayer(ArrayTexture2D* tex);
        void resetTextureLayer(ArrayTexture2D* tex, ArrayTexture2D* newTex);
        void bind();
        inline unsigned int glName() const { return m_glName; }
    private:
        GLuint m_glName;
        unsigned int m_nextLayer = 0;
        std::vector<unsigned int> m_freeLayers = {};
    };
    using Texture2DArrayUPtr = std::unique_ptr<Texture2DArray>;
}
