#include "WorldMaker.h"
#include "Camera.h"
#include "GPUResourceManager.h"
#include "PerlinNoise.h"
#include "RenderingConstants.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "GPUResourceManager.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Vertex.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

#include "Texture2D.h"
#include "NoiseRenderUnit.h"
#include "ChunkRenderUnit.h"

#include "ChunkGeneration.h"

#include "Camera.h"
#include "GlobalLight.h"
#include "CoolTime.h"
#include "Input.h"
#include "Frustum.h"

#include "PRNG.h"

#include <iostream>
#include <memory>
#include "CoreGenerator.h"
#include "BiomeGenerator.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

using namespace WorldMaker;


int main()
{
    Renderer::Init();
	GPUResourceManager::Init();
	ResourceManager::Init();
	Input::SetUp(Renderer::GetWindow());
	BiomeGenerator::addDefaultBiomes();

	int gridWidth = ChunkRenderUnit::s_chunkSide;
    int gridDepth = ChunkRenderUnit::s_chunkSide;

	int world_width = 4;
	int world_height = 4;

	WorldGenerator generator(150, 42);

	std::vector<ChunkRenderUnitUPtr> chunks;

	ChunkGeneration::RegenerateChunks(chunks, generator, world_width, world_height);
	bool doRender2D = false;

	std::vector<double> colors;



	if (doRender2D) {
		colors.reserve(gridWidth * gridDepth);
	}

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
			}
        }
    }


	NoiseRenderUnit noise1 = NoiseRenderUnit(gridWidth, gridDepth, colors);


	if (doRender2D) {
		Renderer::PrepareToDrawNoise(noise1);
	}

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(Renderer::GetWindow(), true);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();

	int chunk_size = ChunkRenderUnit::s_chunkSide;
	int octaves = 4;
	uint64_t seed = 1;
	double frequency = 5;
	double amplitude = 4;
	double lacunarity = 2;
	double persistence = 0.75;

	while (!Renderer::WindowShouldClose())
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		CoolTime::Update();
        Input::Update();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

#define IMGUI_INPUT(var, type) (ImGui::InputScalar("##" #var, type, &var))

        ImGui::Text("FPS: %.1f",
                    ImGui::GetIO().Framerate);

		ImGui::Begin("Generation");
		ImGui::Text("Mesh Size: ");
		IMGUI_INPUT(chunk_size, ImGuiDataType_S32);

		ImGui::Text("Mesh size: ");
		IMGUI_INPUT(world_width, ImGuiDataType_S32);

		ImGui::Text("x");
		IMGUI_INPUT(world_height, ImGuiDataType_S32);

		ImGui::Text("Noise:");
		ImGui::Text("frequency:");
		IMGUI_INPUT(frequency, ImGuiDataType_Double);

		ImGui::Text("amplitude:");
		IMGUI_INPUT(amplitude, ImGuiDataType_Double);

		ImGui::Text("seed:");
		IMGUI_INPUT(seed, ImGuiDataType_U64);

		ImGui::Text("octaves:");
		IMGUI_INPUT(octaves, ImGuiDataType_S32);

		if (ImGui::Button("Generate mesh")) {
			ChunkRenderUnit::s_chunkSide = chunk_size;
			ChunkRenderUnit::s_chunkSide = chunk_size;

			WorldGenerator generator(150,seed);
			chunks.clear();
			ChunkGeneration::RegenerateChunks(chunks, generator, world_width, world_height);
		}

		ImGui::End();

		Camera::UpdateCameraTransform();

		for (auto& ck : chunks) {
		    if (!Camera::CanSeeBox(ck->minPoint(), ck->maxPoint())) continue;
			Renderer::DrawChunkTerrain(*ck);
			Renderer::DrawChunkModels(*ck);
		}

		if (doRender2D) {
			Renderer::DrawNoise(noise1);
		}

		ImGui::Render();
       	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(Renderer::GetWindow());

        glfwPollEvents();
	}
    chunks.clear();
	ResourceManager::Shutdown();
	GPUResourceManager::Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
   	ImGui_ImplGlfw_Shutdown();
   	ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
