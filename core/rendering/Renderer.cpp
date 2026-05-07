#include "Renderer.h"

#include "ChunkRenderUnit.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "DebugUtils.h"
#include "NoiseRenderUnit.h"
#include "OpenGLUtils.h"
#include "ShaderProgram.h"
#include <memory>
#include "ResourceManager.h"

namespace WorldMaker
{
	RendererConfig Renderer::s_config;
	GLFWwindow* Renderer::s_window = nullptr;
	std::map<ShaderProgramType, ShaderProgramSPtr> Renderer::s_shaderProgramsByType = {};

	std::unordered_map<GLenum, std::vector<unsigned int>> Renderer::s_handlesFreeIndexesByType;
    std::unordered_map<GLenum, std::vector<GLuint64>> Renderer::s_handlesByType;
    std::unordered_map<GLenum, GLuint> Renderer::s_buffersByType;
    std::unordered_map<GLenum, int> Renderer::s_bindingsByType;

    bool Renderer::s_texturesUploaded = false;

	void Renderer::Init()
	{
		ASSERT(glfwInit());
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        s_window = glfwCreateWindow(s_config.windowWidth, s_config.windowHeight, s_config.windowTitle, s_config.monitor, s_config.share);
        ASSERT(s_window);

        glfwMakeContextCurrent(s_window);

        glfwSwapInterval(1);

        ASSERT(glewInit() == GLEW_OK);

        if (s_config.depthTest)
        {
            GLCall(glEnable(GL_DEPTH_TEST));
            GLCall(glDepthFunc(GL_LEQUAL));
        }
        if (s_config.blend)
        {
            GLCall(glEnable(GL_BLEND));
            GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        }
        s_shaderProgramsByType[ShaderProgramType::noise] = std::make_shared<ShaderProgram>("core/rendering/shaders/NoiseVertexShader.glsl", "core/rendering/shaders/NoiseFragmentShader.glsl");
        s_shaderProgramsByType[ShaderProgramType::terrain] = std::make_shared<ShaderProgram>("core/rendering/shaders/TerrainVertexShader.glsl", "core/rendering/shaders/TerrainFragmentShader.glsl");

        s_bindingsByType[GL_TEXTURE_2D] = SSBOType::textures2D;
        for (auto& [type, binding] : s_bindingsByType)
        {
            GLCall(glCreateBuffers(1, &s_buffersByType[type]));
        }

        AllocateTextureTypes();

        ResourceManager::Init();
	}

	// Returns the index of the handle in m_handlesByType[type].
    unsigned int Renderer::RegisterTexture(const TextureSPtr& texture)
    {
        GLenum type = texture->texType();

        if (s_handlesByType[type].size() >= maxTextureCountPerType)
        {
            std::cerr << "ERROR: Max texture limit of type " << type << " reached. The limit is " << maxTextureCountPerType << "\n";
            return 0;
        }

        GLuint64 newHandle;
        GLCall(newHandle = glGetTextureHandleARB(texture->glName()));
        ASSERT(newHandle != 0);

        GLCall(glMakeTextureHandleResidentARB(newHandle));

        unsigned int index;

        if (s_handlesFreeIndexesByType[type].size()>0)
        {
        	index = s_handlesFreeIndexesByType[type][0];
            s_handlesByType[type][s_handlesFreeIndexesByType[type][0]] = newHandle;
            s_handlesFreeIndexesByType[type].erase(s_handlesFreeIndexesByType[type].begin());
        }
        else
        {
            s_handlesByType[type].push_back(newHandle);
            index = s_handlesByType[type].size()-1;
        }
        UpdateTextureType(type);
        return index;
    }

    // Should be called before loading any textures (before any UpdateTextures/UpdateTextureType call)
    void Renderer::AllocateTextureTypes()
    {
        for (auto& [type, buffer] : s_buffersByType)
        {
            AllocateTextureType(type);
        }
        s_texturesUploaded = true;
    }

    void Renderer::AllocateTextureType(GLenum type)
    {
        ASSERT(s_handlesByType[type].size() < maxTextureCountPerType);
		ASSERT(!s_texturesUploaded);
		std::cout << "Type: " << type << "\n";
        GLCall(glNamedBufferStorage(
            s_buffersByType[type],
            sizeof(GLuint64) * maxTextureCountPerType,
            NULL,
            GL_DYNAMIC_STORAGE_BIT
        ));
        GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, s_bindingsByType[type], s_buffersByType[type]));
    }

    // Should be used after calling UploadTextures()
    void Renderer::UpdateTextures()
    {
        for (auto& [type, handles] : s_handlesByType)
        {
            UpdateTextureType(type);
        }
	}

	void Renderer::UpdateTextureType(GLenum type)
	{
	    ASSERT(s_handlesByType[type].size() < maxTextureCountPerType);

        GLint size = 0;
        GLCall(glGetNamedBufferParameteriv(s_buffersByType[type], GL_BUFFER_SIZE, &size));

        GLCall(glNamedBufferSubData(
            s_buffersByType[type],
            0,
            sizeof(GLuint64) * s_handlesByType[type].size(),
            (const void*)s_handlesByType[type].data()
        ));
	}

    void Renderer::UnregisterTexture(unsigned int textureIndex, GLenum type)
    {
        if (textureIndex >= s_handlesByType[type].size())
        {
            std::cout << "Warning: trying to unregister invalid texture of index " << textureIndex << "\n";
            return;
        }

        GLuint64 handle = s_handlesByType[type][textureIndex];
        GLCall(glMakeTextureHandleNonResidentARB(handle));

        s_handlesByType[type][textureIndex] = 0;
        s_handlesFreeIndexesByType[type].push_back(textureIndex);

        UpdateTextureType(type);
    }


	void Renderer::PrepareToDrawNoise(NoiseRenderUnit& noise)
	{
		noise.vertexArray->bind();
		Renderer::s_shaderProgramsByType[noise.shaderProgramType]->bind();
		Renderer::s_shaderProgramsByType[noise.shaderProgramType]->setUniform1i("u_texture", 0);

		noise.vertices->bindBufferBase(SSBOType::vertices);
		noise.indices->bindBufferBase(SSBOType::indices);

		noise.noiseTex->bind();
	}
	void Renderer::DrawNoise(const NoiseRenderUnit& noise)
	{
		// Six indices are needed to draw a square
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void Renderer::PrepareToDrawChunk(ChunkRenderUnit& chunk)
	{
		chunk.m_vertexArray->bind();
		Renderer::s_shaderProgramsByType[chunk.shaderProgramType]->bind();
		Renderer::s_shaderProgramsByType[chunk.shaderProgramType]->loadMaterial(*chunk.m_material);
		chunk.m_vertices->bindBufferBase(SSBOType::vertices);
		chunk.m_indices->bindBufferBase(SSBOType::indices);
	}
	void Renderer::DrawChunk(ChunkRenderUnit& chunk)
	{
		glDrawArrays(GL_TRIANGLES, 0, chunk.m_indices->m_data.size());
	}
}
