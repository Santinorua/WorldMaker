#pragma once

#include <iostream>
#include <vector>

#include <Vec2.h>
#include <Vec3.h>

namespace WorldMaker
{
	struct Vertex
	{
		Vec3 m_position;  // x, y, z

		static std::vector<Vertex> CreateQuad(double size);
		operator std::vector<double>() const
		{
			return
			{
				m_position.x, m_position.y, m_position.z
			};
		}
		static std::vector<double> FlattenVertices(const std::vector<Vertex>& vertices);
	};
}
