#pragma once

#include "GL/glew.h"
#include <memory>
#include <string>

namespace WorldMaker
{
    class Texture
    {
    protected:
        std::string m_path;
        GLuint m_glName = 0;

    public:
        virtual ~Texture();

        virtual void bind(unsigned int slot = 0) const = 0;
        virtual void unbind() const = 0;

        virtual GLuint glName() const { return m_glName; }
        virtual GLenum texType() const = 0;

        virtual std::string path() const { return m_path; }
    };

    using TextureUPtr = std::unique_ptr<Texture>;
    using TextureSPtr = std::shared_ptr<Texture>;
    using TextureWPtr = std::weak_ptr<Texture>;
}
