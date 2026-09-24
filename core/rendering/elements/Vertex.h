#pragma once

#include <iostream>
#include <vector>

#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"

namespace WorldMaker
{
	struct MeshVertex
	{
		Vec3 m_position;  // x, y, z
		Vec4 m_color = {1,1,1,1};
		double m_materialLayer = 0;
		Vec2 m_uv;
		Vec3 m_normal;

		static std::vector<MeshVertex> CreateQuad(double size);
		static std::vector<MeshVertex> CreateUnitQuadWithAspectRatio(double width, double height);
		operator std::vector<double>() const
		{
			return
			{
				m_position.x, m_position.y, m_position.z,
				m_color.x, m_color.y, m_color.z, m_color.w,
				m_materialLayer,
				m_uv.x, m_uv.y,
				m_normal.x, m_normal.y, m_normal.z
			};
		}
		static std::vector<double> FlattenVertices(const std::vector<MeshVertex>& vertices);
	};

	struct SkyboxVertex
	{
        Vec3 m_position = {0, 0, 0};  // x, y, z
        Vec4 m_color = {1,1,1,1};
        Vec2 m_uv = {0, 0};
        Vec3 m_normal = {0, 0, 0};

		static std::vector<SkyboxVertex> CreateCube();

		operator std::vector<double>() const
		{
			return
			{
				m_position.x, m_position.y, m_position.z,
				m_color.x, m_color.y, m_color.z, m_color.w,
				m_uv.x, m_uv.y,
				m_normal.x, m_normal.y, m_normal.z
			};
		}
		static std::vector<double> FlattenVertices(const std::vector<SkyboxVertex>& vertices);
	};
}
