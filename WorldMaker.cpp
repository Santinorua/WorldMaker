#include "WorldMaker.h"
#include "Camera.h"
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

    std::vector<Vertex> chunkVertices;
    chunkVertices.reserve(gridWidth * gridDepth);

    FractalNoise fractal(gridWidth, gridDepth, 5, 2, 1, 4, 2.0, 0.75);
    for (int z = 0; z < gridDepth; ++z)
    {
        for (int x = 0; x < gridWidth; ++x)
        {

            double posY = fractal.getNoise(x, z);

	// WorldGenerator worldGenerator(width, height, 42);
	// PerlinNoise perlinNoise(width, height, 10, 1);
	// FractalNoise fractalNoise(width, height, 5, 1, 1, 4, 2.0, 0.5);
	// PerlinNoise3D perlinNoise3D(width, height, 10, 10 , 1);
	// ComplexNoise complexNoise(width, height, -6, {1, 1, 1}, 42);
	// RidgesFolded ridgesFolded = RidgesFolded();
            double hL = fractal.getNoise(x - 1, z);
            double hR = fractal.getNoise(x + 1, z);
            double hD = fractal.getNoise(x, z - 1);
            double hU = fractal.getNoise(x, z + 1);

	// for (int i = 0; i < width*height; i++)
	// {
	// 	// double color = perlinNoise.getPerlinNoise(i%width, i/width) * 0.5 + 0.5;
	// 	// double color = fractalNoise.getNoise(i%width, i/width) * 0.5 + 0.5;
	// 	// double color = perlinNoise3D.getPerlinNoise3D(i%width, i/width, 0) * 0.5 + 0.5;
	// 	// double color = complexNoise.getNoise(i%width, i/width) * 0.5 + 0.5;
	// 	// double color = ridgesFolded.getNoise(i%width, i/width);
	// 	double red = worldGenerator.m_erosion.getNoise(i%width, i/width) * 0.5 + 0.5;
	// 	double blue = worldGenerator.m_continentalness.getNoise(i%width, i/width) * 0.5 + 0.5;
	// 	double color = worldGenerator.m_humidity.getNoise(i%width, i/width) * 0.5 + 0.5;
	// 	colorVector.push_back(blue);
	// 	colorVector.push_back(blue);
	// 	colorVector.push_back(blue);
	// 	colorVector.push_back(1.0);
	// }
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
