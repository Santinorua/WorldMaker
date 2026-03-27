#include "CoolTime.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace WorldMaker
{
	float CoolTime::s_currentFrame = 0.0f;
	float CoolTime::s_lastFrame = 0.0f;

	void CoolTime::Update()
	{
		s_lastFrame = s_currentFrame;
		s_currentFrame = glfwGetTime();
	}
}
