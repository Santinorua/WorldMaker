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
#include "Time.h"
#include "Input.h"

#include "PRNG.h"

#include <iostream>
#include <memory>

using namespace WorldMaker;

int main()
{
	Renderer::Init();
	Input::SetUp(Renderer::GetWindow());

	int width = 200;
	int height = 200;

	PRNG messi;
	messi.seed = 2608;

	std::vector<double> colorVector =
	{

	};

	for (int i = 0; i < width*height; i++)
	{
		double color = static_cast<double>(messi.nextNumber64()%10000)/10000;
		colorVector.push_back(color);
		colorVector.push_back(color);
		colorVector.push_back(color);
		colorVector.push_back(1.0);
	}

	NoiseRenderUnit noise{width, height, colorVector};

    Renderer::PrepareToDrawNoise(noise);

    // std::vector<Vertex> vertices =
    // {
   	// 	// Fila 0 (Y=0, Abajo)
    //     { {-0.5, -0.5, 0.0} }, // V0: Bottom-Left (x=0)
    //     { { 0.5, -0.5, 0.0} }, // V1: Bottom-Right (x=1)

    //     // Fila 1 (Y=1, Arriba)
    //     { {-0.5,  0.5, 0.0} }, // V2: Top-Left (x=0)
    //     { { 0.5,  0.5, 0.0} }  // V3: Top-Right (x=1)

    // };
    // ChunkRenderUnit chunk{vertices};

    // Renderer::PrepareToDrawChunk(chunk);

	while (!Renderer::WindowShouldClose())
	{
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Time::Update();
        Input::Update();

        Camera::UpdateCameraTransform();
        ShaderProgram::s_boundShader->updateCameraMatrices();

        Renderer::DrawNoise(noise);
        // Renderer::DrawChunk(chunk);

        glfwSwapBuffers(Renderer::GetWindow());

        glfwPollEvents();
	}
}
