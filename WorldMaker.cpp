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

	int width = 1000;
	int height = 1000;

	std::vector<double> colorVector =
	{

	};

	PerlinNoise perlinNoise(width, height, 10, 42);
	FractalNoise complexNoise(width, height, 5, 1, 1, 4, 2.0, 0.5);

	for (int i = 0; i < width*height; i++)
	{
		// double color = perlinNoise.getPerlinNoise(i%width, i/width) * 0.5 + 0.5;
		double color = complexNoise.getNoise(i%width, i/width);
		colorVector.push_back(color);
		colorVector.push_back(color);
		colorVector.push_back(color);
		colorVector.push_back(1.0);
	}

	NoiseRenderUnit noise{width, height, colorVector};

    Renderer::PrepareToDrawNoise(noise);
	float Hola = 0;
	uint64_t contador = 2;
	while (!Renderer::WindowShouldClose())
	{
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        CoolTime::Update();
        Input::Update();

        Camera::UpdateCameraTransform();
        ShaderProgram::s_boundShader->updateCameraMatrices();

        Renderer::DrawNoise(noise);
        // Renderer::DrawChunk(chunk);


		// Hola += CoolTime::DeltaTime();
		// if (Hola > 1) {
		// 	// PerlinNoise perlinNoise2(width, height, 10, contador++);
		// 	FractalNoise complexNoise2(width, height, 5, 1, contador++, 4, 2.0, 0.5);
		// 	colorVector = {};
		// 	for (int i = 0; i < width*height; i++)
		// 	{
		// 		// double color = perlinNoise2.getPerlinNoise(i%width, i/width) * 0.5 + 0.5;
		// 		double color = complexNoise2.getNoise(i%width, i/width);
		// 		// if (i/width == 0 )
		// 		// 	std::cout << color << std::endl;
		// 		colorVector.push_back(color);
		// 		colorVector.push_back(color);
		// 		colorVector.push_back(color);
		// 		colorVector.push_back(1.0);
		// 	}
		// 	noise.ChangeNoise(width, height, colorVector);
		// 	Hola = 0;
		// 	Renderer::PrepareToDrawNoise(noise);
		// }
        glfwSwapBuffers(Renderer::GetWindow());

        glfwPollEvents();
	}
}
