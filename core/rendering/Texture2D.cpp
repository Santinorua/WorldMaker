#include "Renderer.h"
#include "ResourceManager.h"
#define STB_IMAGE_STATIC
#include "DebugUtils.h"
#include "OpenGLUtils.h"
#include "Texture2D.h"
#include "FileFunctions.h"
#include "GPUResourceManager.h"
#include "stb_image.h"

namespace WorldMaker
{
	Texture2D::Texture2D(const std::string& relativePath)
		: m_width(0), m_height(0), m_unsignedCharLocalBuffer(nullptr), m_floatLocalBuffer(nullptr)
	{
	    m_path = relativePath;
		std::string globalPath = globalizePath(relativePath);
		GLCall(glGenTextures(1, &m_glName));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_glName));
		stbi_set_flip_vertically_on_load(1);

		m_unsignedCharLocalBuffer = stbi_load(globalPath.c_str(), &m_width, &m_height, &m_BPP, 4);

		if (m_unsignedCharLocalBuffer == NULL)
		{
			const char* reason = stbi_failure_reason();
			if (reason) {
				std::cerr << "Couldn't load Texture2D at path : "<< m_path << "\n The reason was: " << reason << "\n";
			}
			else {
				std::cerr << "Couldn't load Texture2D at path :" << m_path << "\n There was no apparent reason\n";
			}
		}
		else {
			std::cout << "Texture2D at path: " << m_path << " was loaded succesfully" << "\n";
		}

		GLCall(glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, m_width, m_height));
		GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, m_unsignedCharLocalBuffer));

		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

		if (!m_unsignedCharLocalBuffer) std::cout << "Error: image not found at relative path: " << relativePath  << "\n";
		ASSERT(m_unsignedCharLocalBuffer);
		stbi_image_free(m_unsignedCharLocalBuffer);
	}

	Texture2D::Texture2D(const std::string& modelTexPath, const aiTexture* aiTex)
	: m_width(0), m_height(0), m_unsignedCharLocalBuffer(nullptr), m_floatLocalBuffer(nullptr)
	{
        m_path = modelTexPath;

        GLCall(glGenTextures(1, &m_glName));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_glName));

		stbi_set_flip_vertically_on_load(1);

		bool heightZero = false;
	    if (aiTex->mHeight == 0)
    	{
            heightZero = true;
            m_unsignedCharLocalBuffer = stbi_load_from_memory(
                reinterpret_cast<const stbi_uc*>(aiTex->pcData),
                aiTex->mWidth,   // cantidad de bytes
                &m_width,
                &m_height,
                &m_BPP,
                STBI_rgb_alpha
            );
    	}
		else
		{
            m_width = aiTex->mWidth;
            m_height = aiTex->mHeight;
		}

		if (heightZero)
		{
    		if (m_unsignedCharLocalBuffer == NULL)
    		{
    			const char* reason = stbi_failure_reason();
    			if (reason) {
    				std::cerr << "Couldn't load Texture2D from aiTexture at path : "<< m_path << "\n The reason was: " << reason << "\n";
    			}
    			else {
    				std::cerr << "Couldn't load Texture2D from aiTexture at path :" << m_path << "\n There was no apparent reason\n";
    			}
    		}
    		else {
    			std::cout << "Texture2D from aiTexture at path: " << m_path << " was loaded succesfully" << "\n";
    		}
		}

		GLCall(glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, m_width, m_height));
		const void* pixels = heightZero
            ? static_cast<const void*>(m_unsignedCharLocalBuffer)
            : static_cast<const void*>(aiTex->pcData);

		GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels));

		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

		if (heightZero)
		{
    		ASSERT(m_unsignedCharLocalBuffer);
    		stbi_image_free(m_unsignedCharLocalBuffer);
		}
	}

	Texture2D::Texture2D(int width, int height, float* data) : m_width(width), m_height(height), m_unsignedCharLocalBuffer(nullptr), m_floatLocalBuffer(nullptr)
	{
        m_path = "Created without file";
		if (data!=nullptr) m_floatLocalBuffer = data;
		else std::cerr << "Error: the data provided when creating a Texture2D is null!\n";
		GLCall(glGenTextures(1, &m_glName));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_glName));
		GLCall(glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, m_width, m_height));
		GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_FLOAT, m_floatLocalBuffer));

		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		if (!m_floatLocalBuffer) std::cout << "Error: Pixel data is not valid when creating a texture2D" << "\n";
		ASSERT(m_floatLocalBuffer);
		stbi_image_free(m_floatLocalBuffer);
	}

	Texture2D::~Texture2D()
	{
	    ResourceManager::RemoveTexture2DIfExpired(m_path);
		GLCall(glDeleteTextures(1, &m_glName));
	}

	void Texture2D::bind(unsigned int slot /*= 0*/) const
	{
		// OpenGL has slots for textures. "Put this texture into slot 3 please"
		GLCall(glActiveTexture(GL_TEXTURE0 + slot));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_glName));
	}

	void Texture2D::unbind() const
	{
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	}
}
