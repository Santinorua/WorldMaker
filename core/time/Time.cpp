#include "Time.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace WorldMaker
{
	float Time::s_currentFrame = 0.0f;
	float Time::s_lastFrame = 0.0f;

	void Time::Update()
	{
		s_lastFrame = s_currentFrame;
		s_currentFrame = glfwGetTime();
	}
}
