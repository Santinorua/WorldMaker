#pragma once

#include "ChunkRenderUnit.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "NoiseRenderUnit.h"
#include "ShaderProgram.h"
#include "glm/glm.hpp"
#include <map>

namespace WorldMaker
{
	struct RendererConfig
    {
        int windowWidth = 850;
        int windowHeight = 850;
        const char* windowTitle = "Cool Window";


        glm::vec4 clearColor = glm::vec4(0.24f, 0.24f, 0.16f, 1.0f);
        bool useClearColor = true;

        bool depthTest = true;
        bool blend = true;


        GLFWmonitor* monitor = NULL;
        GLFWwindow* share = NULL;

		GLenum blendSFactor = GL_SRC_ALPHA;
		GLenum blendDFactor = GL_ONE_MINUS_SRC_ALPHA;
    };

	class Renderer
	{
	public:
		static RendererConfig s_config;
		static GLFWwindow* s_window;
		static std::map<ShaderProgramType, ShaderProgramSPtr> s_shaderProgramsByType;

		static void Init();
		// Getters for RendererConfig
        static int GetWindowWidth() { return s_config.windowWidth; }
        static int GetWindowHeight()  { return s_config.windowHeight; }
        static const char* GetWindowTitle()  { return s_config.windowTitle; }
        static glm::vec4 GetClearColor()  { return s_config.clearColor; }
        static bool GetUseClearColor()  { return s_config.useClearColor; }
        static bool GetDepthTest()  { return s_config.depthTest; }
        static bool GetBlend()  { return s_config.blend; }
        static GLFWmonitor* GetMonitor()  { return s_config.monitor; }
        static GLFWwindow* GetShare()  { return s_config.share; }
        static GLenum GetBlendSFactor()  { return s_config.blendSFactor; }
        static GLenum GetBlendDFactor()  { return s_config.blendDFactor; }
		static GLFWwindow* GetWindow() { return s_window; }
		static int WindowShouldClose() { return glfwWindowShouldClose(s_window); }

		static void PrepareToDrawNoise(NoiseRenderUnit& noise);
		static void DrawNoise(const NoiseRenderUnit& noise);

		static void PrepareToDrawChunk(ChunkRenderUnit& chunk);
		static void DrawChunk(ChunkRenderUnit& chunk);
	};
}
