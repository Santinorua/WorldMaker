#pragma once

#include <iostream>
#include <vector>

#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"

namespace WorldMaker
{
	struct Vertex
	{
		Vec3 m_position;  // x, y, z
		Vec4 m_color = {1,1,1,1};
		Vec2 m_uv;
		Vec3 m_normal;


		static std::vector<Vertex> CreateQuad(double size);
		static std::vector<Vertex> CreateUnitQuadWithAspectRatio(double width, double height);
		operator std::vector<double>() const
		{
			return
			{
				m_position.x, m_position.y, m_position.z,
				m_color.x, m_color.y, m_color.z, m_color.y,
				m_uv.x, m_uv.y,
				m_normal.x, m_normal.y, m_normal.z
			};
		}
		static std::vector<double> FlattenVertices(const std::vector<Vertex>& vertices);
	};
}
