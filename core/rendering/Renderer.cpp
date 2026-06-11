#include "Renderer.h"

#include "ChunkRenderUnit.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "DebugUtils.h"
#include "NoiseRenderUnit.h"
#include "OpenGLUtils.h"
#include "ShaderProgram.h"
#include "ResourceManager.h"
#include "Pointers.h"

#include <memory>

namespace WorldMaker
{
	RendererConfig Renderer::s_config;
	GLFWwindow* Renderer::s_window = nullptr;
	ShaderProgramSPtr Renderer::s_shaderProgramsByType[2] = {};
	bool Renderer::s_inited = false;

	void Renderer::Init()
	{
	    if (s_inited)
		{
		    std::cerr << "Error: Can't init Renderer again because it has already been initialized!\n";
			return;
		}
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

        s_inited = true;
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
		chunk.m_vertices->bindBufferBase(SSBOType::vertices);
		chunk.m_indices->bindBufferBase(SSBOType::indices);
	}
	void Renderer::DrawChunk(ChunkRenderUnit& chunk)
	{
		glDrawArrays(GL_TRIANGLES, 0, chunk.m_indices->m_data.size());
	}
}
