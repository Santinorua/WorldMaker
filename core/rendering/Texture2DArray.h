#pragma once
#include "OpenGLUtils.h"
#include "Texture2D.h"
#include <vector>

namespace WorldMaker
{
    class Texture2DArray
    {
    public:
        Texture2DArray(int width, int height, int layers);
        unsigned int pushTexture(Texture2D* texture);
        void bind();
        inline unsigned int glName() const { return m_glName; }
    private:
        GLuint m_glName;
        unsigned int m_nextLayer = 0;
        std::vector<unsigned int> m_freeLayers = {};
    };
    using Texture2DArrayUPtr = std::unique_ptr<Texture2DArray>;
}
