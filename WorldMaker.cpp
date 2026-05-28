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

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

using namespace WorldMaker;

void generate_mesh(ChunkRenderUnit& render_unit, FractalNoise& noise) {
    std::vector<Vertex> chunkVertices;
    chunkVertices.reserve(ChunkRenderUnit::chunkWidth * ChunkRenderUnit::chunkHeight);

    for (int z = 0; z < ChunkRenderUnit::chunkHeight; ++z)
    {
        for (int x = 0; x < ChunkRenderUnit::chunkWidth; ++x)
        {

            double posY = noise.getNoise(x, z);

            double hL = noise.getNoise(x - 1, z);
            double hR = noise.getNoise(x + 1, z);
            double hD = noise.getNoise(x, z - 1);
            double hU = noise.getNoise(x, z + 1);

            glm::vec3 normal;
            normal.x = static_cast<float>(hL - hR);
            normal.y = static_cast<float>(2.0 * 1);
            normal.z = static_cast<float>(hD - hU);

            normal = glm::normalize(normal);

            Vertex v;
            v.m_position = { static_cast<float>(x), static_cast<float>(posY), static_cast<float>(z) };
            v.m_normal = normal;

            chunkVertices.push_back(v);
        }
    }

	render_unit.ChangeVertices(chunkVertices);

    Renderer::PrepareToDrawChunk(render_unit);
}

int main()
{
	Renderer::Init();
	Input::SetUp(Renderer::GetWindow());

	int gridWidth = ChunkRenderUnit::chunkWidth;
    int gridDepth = ChunkRenderUnit::chunkHeight;

    std::vector<Vertex> chunkVertices;
    chunkVertices.reserve(gridWidth * gridDepth);

    FractalNoise fractal(gridWidth, gridDepth, 5, 2, 1, 4, 2.0, 0.75);
    for (int z = 0; z < gridDepth; ++z)
    {
        for (int x = 0; x < gridWidth; ++x)
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
            v.m_color = {1,1,1,1};
            v.m_uv = { static_cast<float>(x) / 10.0f, static_cast<float>(z) / 10.0f };
            v.m_position = { static_cast<float>(x), static_cast<float>(posY), static_cast<float>(z) };
            v.m_normal = normal;

            chunkVertices.push_back(v);
        }
    }

    ChunkRenderUnit chunk{chunkVertices};

    Renderer::PrepareToDrawChunk(chunk);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(Renderer::GetWindow(), true);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();

	int chunk_size = ChunkRenderUnit::chunkHeight;
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

		ImGui::Begin("Hola");
		ImGui::Text("Mesh Size: ");
		ImGui::InputInt("##mesh_size", &chunk_size);
		if (ImGui::Button("Generate mesh")) {
			ChunkRenderUnit::chunkWidth = chunk_size;
			ChunkRenderUnit::chunkHeight = chunk_size;

			FractalNoise fractal(ChunkRenderUnit::chunkWidth, ChunkRenderUnit::chunkHeight, 5, 2, 1, 4, 2.0, 0.75);
			generate_mesh(chunk, fractal);
		}
		ImGui::End();

        Camera::UpdateCameraTransform();
        ShaderProgram::s_boundShader->updateCameraMatrices();
        GlobalLight::LoadLightSettings();

        Renderer::DrawChunk(chunk);
		ImGui::Render();
       	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(Renderer::GetWindow());

        glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
   	ImGui_ImplGlfw_Shutdown();
   	ImGui::DestroyContext();
}
