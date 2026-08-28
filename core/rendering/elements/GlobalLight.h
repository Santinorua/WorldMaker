#pragma once

#include "ShaderProgram.h"
#include "Renderer.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Vec4.h"
#include "Vec3.h"

namespace WorldMaker
{
	class GlobalLight
	{

	private:
		static Vec4 s_color;
		static float s_lightOffset;
	public:
		static Vec3 s_lightDir;
		static void SetLightColor(Vec4 color);

		static void SetLightOffset(float offset);

		static void LoadLightSettings();

	};
}
