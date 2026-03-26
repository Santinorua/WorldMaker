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

	int width = 500;
	int height = 500;

	std::vector<double> colorVector =
	{

	};

	PerlinNoise perlinNoise(width, height, 10.0, 42);
	for (int i = 0; i < width*height; i++)
	{
		double color = perlinNoise.getPerlinNoise(i%width, i/width);
		if (i/width == 0 )
			std::cout << color << std::endl;
		colorVector.push_back(color);
		colorVector.push_back(color);
		colorVector.push_back(color);
		colorVector.push_back(1.0);
	}

	NoiseRenderUnit noise{width, height, colorVector};

    Renderer::PrepareToDrawNoise(noise);

	while (!Renderer::WindowShouldClose())
	{
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        CoolTime::Update();
        Input::Update();

        Camera::UpdateCameraTransform();
        ShaderProgram::s_boundShader->updateCameraMatrices();

        Renderer::DrawNoise(noise);
        // Renderer::DrawChunk(chunk);

        glfwSwapBuffers(Renderer::GetWindow());

        glfwPollEvents();
	}
}
