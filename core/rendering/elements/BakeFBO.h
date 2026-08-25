#pragma once

#include "OpenGLUtils.h"

#include <vector>

namespace WorldMaker
{
    class ChunkRenderUnit;

    struct BakeFBO
    {
        GLuint fbo = 0;
        GLuint colorTexture = 0;
        int size = 0;

        void Init(int textureSize);
        void Reset();
        std::vector<unsigned char> BakeChunkTerrain(ChunkRenderUnit& chunk);
    };
}
