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

#include "Camera.h"
#include "GlobalLight.h"
#include "CoolTime.h"
#include "Input.h"

#include "PRNG.h"

#include <iostream>
#include <memory>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

using namespace WorldMaker;

ChunkRenderUnit* generate_chunk(FractalNoise& fractal, float x_offset, float z_offset)
{
	int gridWidth = ChunkRenderUnit::chunkWidth;
    int gridDepth = ChunkRenderUnit::chunkHeight;

    // std::vector<Vertex> chunkVertices;
    // chunkVertices.reserve(gridWidth * gridDepth);

    // FractalNoise fractal(gridWidth, gridDepth, 5, 2, 1, 4, 2.0, 0.75);
    // for (int z = 0; z < gridDepth; ++z)
    // {
    //     for (int x = 0; x < gridWidth; ++x)
    //     {

    //         double posY = fractal.getNoise(x, z);

    //         double hL = fractal.getNoise(x - 1, z);
    //         double hR = fractal.getNoise(x + 1, z);
    //         double hD = fractal.getNoise(x, z - 1);
    //         double hU = fractal.getNoise(x, z + 1);

    //         glm::vec3 normal;
    //         normal.x = static_cast<float>(hL - hR);
    //         normal.y = static_cast<float>(2.0 * 1);
    //         normal.z = static_cast<float>(hD - hU);

    //         normal = glm::normalize(normal);

    //         Vertex v;
    //         v.m_color = {1,1,1,1};
    //         v.m_uv = { static_cast<float>(x) / 10.0f, static_cast<float>(z) / 10.0f };
    //         v.m_position = { static_cast<float>(x), static_cast<float>(posY), static_cast<float>(z) };
    //         v.m_normal = normal;

    //         chunkVertices.push_back(v);
    //     }
    // }

    // ChunkRenderUnit chunk{chunkVertices};

    // Renderer::PrepareToDrawChunk(chunk);

    std::vector<double> pixels;
    int width = 10;
    int height = 10;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            pixels.push_back(1);
            pixels.push_back(1);
            pixels.push_back(1);
            pixels.push_back(1);
        }
    }
    NoiseRenderUnit noise{width, height, pixels};
    std::vector<Vertex> chunkVertices;
    chunkVertices.reserve(ChunkRenderUnit::chunkWidth * ChunkRenderUnit::chunkHeight);

    for (int z = z_offset * ChunkRenderUnit::chunkHeight - (z_offset != 0); z < ChunkRenderUnit::chunkHeight * (z_offset + 1) - (z_offset != 0); ++z)
    {
        for (int x = x_offset * ChunkRenderUnit::chunkWidth - (x_offset != 0); x < ChunkRenderUnit::chunkWidth * (x_offset + 1) - (x_offset != 0); ++x)
        {

            double posY = fractal.getNoise(x, z);

            double hL = fractal.getNoise(x - 1, z);
            double hR = fractal.getNoise(x + 1, z);
            double hD = fractal.getNoise(x, z - 1);
            double hU = fractal.getNoise(x, z + 1);

			glm::vec3 normal;
			normal.x = static_cast<float>(hL - hR);
			normal.y = static_cast<float>(2.0 * 1);
			normal.z = static_cast<float>(hD - hU);

			normal = glm::normalize(normal);

            Vertex v;
            v.m_color = {0.5,0.5,0.5,1};
            v.m_uv = { static_cast<float>(x) / 10.0f, static_cast<float>(z) / 10.0f };
			v.m_position = { static_cast<float>(x), static_cast<float>(posY), static_cast<float>(z)};
			v.m_normal = normal;


			if (x % ChunkRenderUnit::chunkWidth == 0 || x % ChunkRenderUnit::chunkWidth == ChunkRenderUnit::chunkWidth - 1) {
				v.m_color.x = 1;
			}

			if (z % ChunkRenderUnit::chunkHeight == 0 || z % ChunkRenderUnit::chunkHeight == ChunkRenderUnit::chunkHeight - 1) {
				v.m_color.y = 1;
			}

			chunkVertices.push_back(v);
        }
    }

    return new ChunkRenderUnit{chunkVertices};
}

void regenerate_chunks(std::vector<ChunkRenderUnit*>& chunks, FractalNoise& fractal, int width, int height)
{
	for (ChunkRenderUnit *ck : chunks) {
		delete ck;
	}
	chunks.clear();

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			chunks.push_back(generate_chunk(fractal, x, y));
		}
	}
}

int main()
{
    Renderer::Init();
	GPUResourceManager::Init();
	ResourceManager::Init();
	Input::SetUp(Renderer::GetWindow());

	int gridWidth = ChunkRenderUnit::chunkWidth;
    int gridDepth = ChunkRenderUnit::chunkHeight;

	int world_width = 4;
	int world_height = 4;

    FractalNoise fractal(ChunkRenderUnit::chunkWidth * 4, ChunkRenderUnit::chunkHeight * 4, 5, 4, 1, 4, 2.0, 0.75);

	std::vector<ChunkRenderUnit*> chunks;

	regenerate_chunks(chunks, fractal, world_width, world_height);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(Renderer::GetWindow(), true);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();

	int chunk_size = ChunkRenderUnit::chunkHeight;
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
			ChunkRenderUnit::chunkWidth = chunk_size;
			ChunkRenderUnit::chunkHeight = chunk_size;

			FractalNoise fractal(ChunkRenderUnit::chunkWidth * 4, ChunkRenderUnit::chunkHeight * 4, frequency, amplitude, seed, octaves, lacunarity, persistence);
			for (ChunkRenderUnit* ck : chunks) {
				delete ck;
			}
			chunks.clear();
			regenerate_chunks(chunks, fractal, world_width, world_height);
		}
		ImGui::End();

		Renderer::s_shaderProgramsByType[ShaderProgramType::terrain]->bind();

		Camera::UpdateCameraTransform();
		ShaderProgram::s_boundShader->updateCameraMatrices();
        GlobalLight::LoadLightSettings();

		for (ChunkRenderUnit* ck : chunks) {
			Renderer::PrepareToDrawChunk(*ck);
			GPUResourceManager::PrepareToDraw();
			Renderer::DrawChunk(*ck);
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
