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
#include "ui.h"

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
	int render_distance = 4;

	WorldGenerator generator(150, 42);

	ChunkGeneration::ChunkArray chunks;

	ChunkGeneration::RegenerateChunks(chunks, generator, render_distance, Camera::Position());

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


	ui::init();

	int chunk_size = ChunkRenderUnit::s_chunkSide;
	uint64_t seed = 42;

	bool quit = false;
	while (!Renderer::WindowShouldClose() && !quit)
	{
		ChunkGeneration::RegenerateChunks(chunks, generator, render_distance, Camera::Position());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		CoolTime::Update();
        Input::Update();

		ui::begin();

		ui::DockSpace(quit);

		ui::DebugWindow(render_distance, chunks);

		bool redraw = ui::GenerationWindow(chunk_size, world_width, seed, render_distance, chunks, generator);
		ui::BiomesWindow();

		Camera::UpdateCameraTransform();

		for (auto& ck : chunks) {
		    if (!Camera::CanSeeBox(ck.second->minPoint(), ck.second->maxPoint())) continue;
			Renderer::DrawChunkTerrain(*ck.second);
			Renderer::DrawChunkModels(*ck.second);
		}
		if (doRender2D) {
			Renderer::DrawNoise(noise1);
		}

		ui::end();
		glfwSwapBuffers(Renderer::GetWindow());

		glfwPollEvents();
	}

	for (auto& ck : chunks) {
		delete ck.second;
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
