#include "Vertex.h"

namespace WorldMaker
{
	std::vector<Vertex> Vertex::CreateQuad(double size)
	{
		double half = size / 2;
		Vertex v0;
		v0.m_position = { -half, -half, 0.0f };
		Vertex v1;
		v1.m_position = { half, -half, 0.0f };
		Vertex v2;
		v2.m_position = { half, half, 0.0f };
		Vertex v3;
		v3.m_position = { -half, half, 0.0f };
		return { v0, v1, v2, v3 };
	}

	std::vector<double> Vertex::FlattenVertices(const std::vector<Vertex>& vertices)
	{
		std::vector<double> result;
		result.reserve(vertices.size() * (3 + 2 + 3));  // pos + uv + normal

		for (const auto& vertex : vertices)
		{
			std::vector<double> flatVertex = vertex;
			result.insert(result.end(), flatVertex.begin(), flatVertex.end());
		}
		return result;
	}
}
