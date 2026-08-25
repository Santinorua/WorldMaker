#include "BakeFBO.h"

#include "ChunkRenderUnit.h"
#include "DebugUtils.h"
#include "OpenGLUtils.h"
#include "Texture.h"
#include <GL/gl.h>
#include "stb_image_write.h"
namespace WorldMaker
{
    void BakeFBO::Init(int textureSize)
    {
        size = textureSize;

        GLCall(glGenTextures(1, &colorTexture));
        GLCall(glBindTexture(GL_TEXTURE_2D, colorTexture));
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        GLCall(glGenFramebuffers(1, &fbo));
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
        GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0));

        GLenum status;
        GLCall(status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "Bake FOB incomplete: " << status << "\n";
        }

        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }
    std::vector<unsigned char> BakeFBO::BakeChunkTerrain(ChunkRenderUnit& chunk)
    {
        ASSERT(size>0);
        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
        GLint prevViewport[4];
        GLCall(glGetIntegerv(GL_VIEWPORT, prevViewport));
        GLCall(glViewport(0, 0, size, size));
        GLCall(glClearColor(0,0,0,0));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "FBO incompleto en draw! status=" << status << "\n";

        Renderer::DrawBakedTerrain(chunk);

        std::vector<unsigned char> pixels(size * size * 4);
        GLCall(glReadPixels(0, 0, size, size, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data()));

        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        GLCall(glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]));

        return pixels;
    }
    void BakeFBO::Reset()
    {
        GLCall(glDeleteFramebuffers(1, &fbo));
        GLCall(glDeleteTextures(1, &colorTexture));
    }
}
