#include "WorldMaker.h"
#include "Camera.h"
#include "PerlinNoise.h"
#include "RenderingConstants.h"
#include "Renderer.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Vertex.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

#include "Texture2D.h"
#include "NoiseRenderUnit.h"
#include "ChunkRenderUnit.h"

#include "Camera.h"
#include "GlobalLight.h"
#include "CoolTime.h"
#include "Input.h"

#include "PRNG.h"

#include <iostream>
#include <memory>
#include "CoreGenerator.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

using namespace WorldMaker;

int main()
{
	Renderer::Init();
	Input::SetUp(Renderer::GetWindow());

	int gridWidth = ChunkRenderUnit::chunkWidth;
    int gridDepth = ChunkRenderUnit::chunkHeight;

	bool doRender2D = false;

    std::vector<Vertex> chunkVertices;
	std::vector<double> colors;

	if (doRender2D) {
		colors.reserve(gridWidth * gridDepth);
	} else {
		chunkVertices.reserve(gridWidth * gridDepth);
	}


    // FractalNoise fractal(gridWidth, gridDepth, 5, 2, 1, 4, 2.0, 0.75);
	WorldGenerator generator(150, 42);
	// PerlinNoise perlin = PerlinNoise(40, 42);
	for (int z = 0; z < gridDepth; ++z)
    {
        for (int x = 0; x < gridWidth; ++x)
        {


			if (doRender2D) {
				double color = generator.getVertex(x, z).m_position.y / generator.m_yScale;
				colors.push_back(color);
				colors.push_back(color);
				colors.push_back(color);
				colors.push_back(1.0);
			} else {
				chunkVertices.push_back(generator.getVertex(x, z));
			}

        }
    }

	for (int z = 0; z < gridDepth; ++z) {
		for (int x = 0; x < gridWidth; ++x) {


			float hL = x != 0 ? chunkVertices[z*gridWidth + x-1].m_position.y : generator.getVertex(x-1, z).m_position.y;
			float hR = x != gridWidth-1 ? chunkVertices[z*gridWidth + x+1].m_position.y : generator.getVertex(x+1, z).m_position.y;
			float hU = z != 0 ? chunkVertices[(z-1)*gridWidth + x].m_position.y : generator.getVertex(x, z-1).m_position.y;
			float hD = z != gridDepth-1 ? chunkVertices[(z+1)*gridWidth + x].m_position.y : generator.getVertex(x, z+1).m_position.y;

			glm::vec3 normal;
			normal.x = static_cast<float>(hL - hR);
			normal.y = static_cast<float>(2.0 * 1);
			normal.z = static_cast<float>(hD - hU);

			normal = glm::normalize(normal);

			chunkVertices[z*gridWidth + x].m_normal = normal;
		}
	}

	ChunkRenderUnit chunk{chunkVertices};

	NoiseRenderUnit noise1 = NoiseRenderUnit(gridWidth, gridDepth, colors);


	if (doRender2D) {
		Renderer::PrepareToDrawNoise(noise1);
	} else {
		Renderer::PrepareToDrawChunk(chunk);
	}


    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(Renderer::GetWindow(), true);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();

	while (!Renderer::WindowShouldClose())
	{
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        CoolTime::Update();
        Input::Update();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Text("FPS: %.1f",
                    ImGui::GetIO().Framerate);

        Camera::UpdateCameraTransform();
        ShaderProgram::s_boundShader->updateCameraMatrices();
        GlobalLight::LoadLightSettings();

		if (doRender2D) {
			Renderer::DrawNoise(noise1);
		} else {
			Renderer::DrawChunk(chunk);
		}

		ImGui::Render();
       	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(Renderer::GetWindow());

        glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
   	ImGui_ImplGlfw_Shutdown();
   	ImGui::DestroyContext();
}
