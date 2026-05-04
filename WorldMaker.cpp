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
            v.m_position = { static_cast<float>(x), static_cast<float>(posY), static_cast<float>(z) };
            v.m_normal = normal;

            chunkVertices.push_back(v);
        }
    }

    ChunkRenderUnit chunk{chunkVertices};

    Renderer::PrepareToDrawChunk(chunk);

	while (!Renderer::WindowShouldClose())
	{
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        CoolTime::Update();
        Input::Update();

        Camera::UpdateCameraTransform();
        ShaderProgram::s_boundShader->updateCameraMatrices();
        GlobalLight::LoadLightSettings();

        Renderer::DrawChunk(chunk);

		// Hola += CoolTime::DeltaTime();
		// if (Hola > 1) {
		// 	PerlinNoise perlinNoise2(width, height, 10, contador++);
		// 	// FractalNoise fractalNoise2(width, height, 5, 1, contador++, 4, 2.0, 0.5);
		// 	colorVector = {};
		// 	for (int i = 0; i < width*height; i++)
		// 	{
		// 		double color = perlinNoise2.getPerlinNoise(i%width, i/width) * 0.5 + 0.5;
		// 		// double color = fractalNoise2.getNoise(i%width, i/width) * 0.5 + 0.5;
		// 		// double color = perlinNoise3D.getPerlinNoise3D(i%width, i/width, contador) * 0.5 + 0.5;
		// 		// if (i/width == 0 )
		// 		// 	std::cout << color << std::endl;
		// 		colorVector.push_back(color);
		// 		colorVector.push_back(color);
		// 		colorVector.push_back(color);
		// 		colorVector.push_back(1.0);
		// 	}
		// 	// contador++;
		// 	noise.ChangeNoise(width, height, colorVector);
		// 	Hola = 0;
		// 	// Renderer::PrepareToDrawNoise(noise);
		// }
        glfwSwapBuffers(Renderer::GetWindow());

        glfwPollEvents();
	}
}
