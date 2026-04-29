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

using namespace WorldMaker;

int main()
{
	Renderer::Init();
	Input::SetUp(Renderer::GetWindow());

	int gridWidth = 10;
    int gridDepth = 10;

    double planeSizeX = 10.0;
    double planeSizeZ = 10.0;

    double stepX = planeSizeX / (gridWidth - 1);
    double stepZ = planeSizeZ / (gridDepth - 1);

    std::vector<Vertex> chunkVertices;
    chunkVertices.reserve(gridWidth * gridDepth);

    auto getHeight = [](double x, double z) -> double {
        double y = std::sin(x * 0.8) * std::cos(z * 0.8) * 1.5;
        y += std::sin(x * 2.5) * 0.3;
        return y;
    };

    const double EPSILON = 0.01;

    for (int z = 0; z < gridDepth; ++z)
    {
        for (int x = 0; x < gridWidth; ++x)
        {
            double posX = - (planeSizeX / 2.0) + (x * stepX);
            double posZ = - (planeSizeZ / 2.0) + (z * stepZ);

            double posY = getHeight(posX, posZ);

            double hL = getHeight(posX - EPSILON, posZ);
            double hR = getHeight(posX + EPSILON, posZ);
            double hD = getHeight(posX, posZ - EPSILON);
            double hU = getHeight(posX, posZ + EPSILON);

            glm::vec3 normal;
            normal.x = static_cast<float>(hL - hR);
            normal.y = static_cast<float>(2.0 * EPSILON);
            normal.z = static_cast<float>(hD - hU);

            normal = glm::normalize(normal);

            Vertex v;
            v.m_position = { static_cast<float>(posX), static_cast<float>(posY), static_cast<float>(posZ) };
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
