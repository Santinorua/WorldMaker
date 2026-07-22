#include "GlobalLight.h"
#include "glm/fwd.hpp"

namespace WorldMaker
{
	Vec4 GlobalLight::s_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	float GlobalLight::s_lightOffset = 0.1f;
	Vec3 GlobalLight::s_pos = {0.0f, 500.0f, 0.0f};
	void GlobalLight::SetLightColor(Vec4 color)
	{
		s_color = color;
	}
	void GlobalLight::SetLightOffset(float offset)
	{
		s_lightOffset = offset;
	}

	void GlobalLight::LoadLightSettings()
	{
	    ShaderProgram::s_boundShader->setUniform4f("u_globalLightColor", s_color.x, s_color.y, s_color.z, s_color.w);
		ShaderProgram::s_boundShader->setUniform1f("u_lightOffset", s_lightOffset);
		ShaderProgram::s_boundShader->setUniform3f("u_globalLightPos", s_pos.x, s_pos.y, s_pos.z);
	}
}
