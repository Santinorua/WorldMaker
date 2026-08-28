#include "Renderer.h"

#include "ChunkRenderUnit.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "DebugUtils.h"
#include "NoiseRenderUnit.h"
#include "OpenGLUtils.h"
#include "RenderingConstants.h"
#include "ShaderProgram.h"
#include "ResourceManager.h"
#include "Pointers.h"
#include "Camera.h"
#include "GlobalLight.h"
#include "GPUResourceManager.h"
#include "WorldWater.h"

#include <memory>

namespace WorldMaker
{
	RendererConfig Renderer::s_config;
	GLFWwindow* Renderer::s_window = nullptr;
	BakeFBO Renderer::s_bakeFBO;
	ShaderProgramSPtr Renderer::s_shaderProgramsByType[shadersAmount] = {};
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

        glfwSetFramebufferSizeCallback(s_window, framebuffer_size_callback);

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
        // GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
        GLCall(glEnable(GL_CULL_FACE));
        GLCall(glCullFace(GL_BACK));
        GLCall(glFrontFace(GL_CCW));

        s_bakeFBO.Init(2048);

        s_shaderProgramsByType[ShaderProgramType::noise] = std::make_shared<ShaderProgram>("core/rendering/shaders/NoiseVertexShader.glsl", "core/rendering/shaders/NoiseFragmentShader.glsl");
        s_shaderProgramsByType[ShaderProgramType::terrain] = std::make_shared<ShaderProgram>("core/rendering/shaders/TerrainVertexShader.glsl", "core/rendering/shaders/TerrainFragmentShader.glsl");
        s_shaderProgramsByType[ShaderProgramType::model] = std::make_shared<ShaderProgram>("core/rendering/shaders/ModelVertexShader.glsl", "core/rendering/shaders/ModelFragmentShader.glsl");
        s_shaderProgramsByType[ShaderProgramType::baking] = std::make_shared<ShaderProgram>("core/rendering/shaders/BakingVertexShader.glsl", "core/rendering/shaders/BakingFragmentShader.glsl");
        s_shaderProgramsByType[ShaderProgramType::water] = std::make_shared<ShaderProgram>("core/rendering/shaders/WaterVertexShader.glsl", "core/rendering/shaders/WaterFragmentShader.glsl");

        WorldWater::Init();

        s_inited = true;
	}

	void Renderer::PrepareToDrawNoise(NoiseRenderUnit& noise)
	{
		noise.vertexArray->bind();
		ShaderProgramSPtr shaderProgram = Renderer::s_shaderProgramsByType[noise.shaderProgramType];
		shaderProgram->bind();
		shaderProgram->setUniform1i("u_texture", 0);

		noise.vertices->bindBufferBase(SSBOType::vertices);
		noise.indices->bindBufferBase(SSBOType::indices);

		noise.noiseTex->bind();
	}


	void Renderer::DrawNoise(const NoiseRenderUnit& noise)
	{
		// Six indices are needed to draw a square
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void Renderer::DrawBakedTerrain(ChunkRenderUnit& chunk)
	{
    	GLCall(glDisable(GL_DEPTH_TEST));
        GLCall(glDisable(GL_CULL_FACE));
		ShaderProgramSPtr shaderProgram = Renderer::s_shaderProgramsByType[ShaderProgramType::baking];
		shaderProgram->bind();
        shaderProgram->setUniform1f("u_chunkSize", ChunkRenderUnit::s_chunkSide);
        shaderProgram->setUniform2f("u_chunkWorldOrigin", chunk.minPoint().x, chunk.minPoint().z);
        GPUResourceManager::PrepareToDrawTerrain();
		chunk.m_vertexArray->bind();
		chunk.m_vertices->bindBufferBase(SSBOType::vertices);
		chunk.m_indices->bindBufferBase(SSBOType::indices);
		glDrawArrays(GL_TRIANGLES, 0, chunk.m_indices->m_data.size());
		GLCall(glEnable(GL_CULL_FACE));
		GLCall(glEnable(GL_DEPTH_TEST));
	}

	void Renderer::DrawWater()
	{
	    GLCall(glDisable(GL_CULL_FACE));
		ShaderProgramSPtr shaderProgram = s_shaderProgramsByType[ShaderProgramType::terrain];
		shaderProgram->bind();
		ShaderProgram::s_boundShader->updateCameraMatrices();
		GlobalLight::LoadLightSettings();
        GPUResourceManager::PrepareToDrawTerrain();
	    WorldWater::s_vertexArray->bind();
        WorldWater::s_vertices->bindBufferBase(SSBOType::vertices);
        WorldWater::s_indices->bindBufferBase(SSBOType::indices);
        glDrawArrays(GL_TRIANGLES, 0, WorldWater::s_indices->m_data.size());
	}

	void Renderer::DrawChunkTerrain(ChunkRenderUnit& chunk)
	{
	    GLCall(glEnable(GL_CULL_FACE));
	    Renderer::s_shaderProgramsByType[ShaderProgramType::terrain]->bind();
		ShaderProgram::s_boundShader->updateCameraMatrices();
        GlobalLight::LoadLightSettings();
        GPUResourceManager::PrepareToDrawTerrain();
		chunk.m_vertexArray->bind();
		chunk.m_vertices->bindBufferBase(SSBOType::vertices);
		chunk.m_indices->bindBufferBase(SSBOType::indices);
		glDrawArrays(GL_TRIANGLES, 0, chunk.m_indices->m_data.size());
	}
	void Renderer::DrawChunkModels(ChunkRenderUnit& chunk)
	{
	    GLCall(glDisable(GL_CULL_FACE));
        Renderer::s_shaderProgramsByType[ShaderProgramType::model]->bind();
		ShaderProgram::s_boundShader->updateCameraMatrices();
        GlobalLight::LoadLightSettings();
	    for (auto& [modelId, pair] : chunk.m_models.m_modelInstancesSSBO)
    	{
            auto& [model, matricesSSBO] = pair;
            for (MeshSPtr mesh : model->m_meshes)
            {
                ShaderProgram::s_boundShader->loadMeshMaterial(mesh->m_material.get());
                mesh->m_vertexArray->bind();
                matricesSSBO->submitData();
                matricesSSBO->bindBufferBase(SSBOType::modelMatrices);
                mesh->submitData();
                mesh->bindBuffersBase();
                GLCall(glDrawArraysInstanced(GL_TRIANGLES, 0, mesh->m_indices->currentElements(), matricesSSBO->m_data.size()));
            }
    	}
	}

	void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        s_config.windowWidth = width;
        s_config.windowHeight = height;
        glViewport(0, 0, width, height);
        if (height==0) height = 1;
        float newAspectRatio = static_cast<float>(width) / static_cast<float>(height);
        Camera::SetAspectRatio(newAspectRatio);
    }
}
