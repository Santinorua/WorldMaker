#include "Vertex.h"

namespace WorldMaker
{
	std::vector<MeshVertex> MeshVertex::CreateQuad(double size)
	{
		double half = size / 2;
		MeshVertex v0;
		v0.m_position = { -half, -half, 0.0f };
		v0.m_uv = {0.0f, 0.0f};
		MeshVertex v1;
		v1.m_position = { half, -half, 0.0f };
		v1.m_uv = {1.0f,0.0f};
		MeshVertex v2;
		v2.m_position = { half, half, 0.0f };
		v2.m_uv = {1.0f,1.0f};
		MeshVertex v3;
		v3.m_position = { -half, half, 0.0f };
		v3.m_uv = {0.0f, 1.0f};
		return { v0, v1, v2, v3 };
	}

	std::vector<SkyboxVertex> SkyboxVertex::CreateCube()
	{
    	std::vector<SkyboxVertex> cube;

        // Face front (Z-)
		cube.push_back({ {-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}});
		cube.push_back({ { 1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}});
		cube.push_back({ { 1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}});
		cube.push_back({ {-1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}});

		// Face back (Z+)
		cube.push_back({ {-1.0f, -1.0f,  1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}});
		cube.push_back({ { 1.0f, -1.0f,  1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}});
		cube.push_back({ { 1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}});
		cube.push_back({ {-1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}});

		// Face left (X-)
		cube.push_back({ {-1.0f, -1.0f,  1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}});
		cube.push_back({ {-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}});
		cube.push_back({ {-1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}});
		cube.push_back({ {-1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}});

		// Face right (X+)
		cube.push_back({ { 1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}});
		cube.push_back({ { 1.0f, -1.0f,  1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}});
		cube.push_back({ { 1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}});
		cube.push_back({ { 1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}});

		// Face bottom (Y-)
		cube.push_back({ {-1.0f, -1.0f,  1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}});
		cube.push_back({ { 1.0f, -1.0f,  1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}});
		cube.push_back({ { 1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}});
		cube.push_back({ {-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}});

		// Face top (Y+)
		cube.push_back({ {-1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}});
		cube.push_back({ { 1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}});
		cube.push_back({ { 1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}});
		cube.push_back({ {-1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}});

    	return cube;
	};

	std::vector<MeshVertex> MeshVertex::CreateUnitQuadWithAspectRatio(double width, double height)
	{
	    double aspectRatio = width / height;

	    double halfWidth = 0.0;
	    double halfHeight = 0.0;
		if (aspectRatio >= 1.0)
		{
			halfWidth = 1.0;
			halfHeight = 1.0 / aspectRatio;
		}
	    else
	    {
	        halfWidth = 1.0 * aspectRatio;
	        halfHeight = 1.0;
	    }

		MeshVertex v0;
		v0.m_position = { -halfWidth, -halfWidth, 0.0f };
		v0.m_uv = {0.0f, 0.0f};
		MeshVertex v1;
		v1.m_position = { halfWidth, -halfWidth, 0.0f };
		v1.m_uv = {1.0f,0.0f};
		MeshVertex v2;
		v2.m_position = { halfWidth, halfWidth, 0.0f };
		v2.m_uv = {1.0f,1.0f};
		MeshVertex v3;
		v3.m_position = { -halfWidth, halfWidth, 0.0f };
		v3.m_uv = {0.0f, 1.0f};

		return { v0, v1, v2, v3 };
	}

	std::vector<double> MeshVertex::FlattenVertices(const std::vector<MeshVertex>& vertices)
	{
		std::vector<double> result;
		result.reserve(vertices.size() * (3 + 4 + 1 + 2 + 3));  // pos + color + materialId + uv + normal

		for (const auto& vertex : vertices)
		{
			std::vector<double> flatVertex = vertex;
			result.insert(result.end(), flatVertex.begin(), flatVertex.end());
		}
		return result;
	}
	std::vector<double> SkyboxVertex::FlattenVertices(const std::vector<SkyboxVertex>& vertices)
	{
		std::vector<double> result;
		result.reserve(vertices.size() * (3 + 4 + 2 + 3));  // pos + color + uv + normal

		for (const auto& vertex : vertices)
		{
			std::vector<double> flatVertex = vertex;
			result.insert(result.end(), flatVertex.begin(), flatVertex.end());
		}
		return result;
	}
}
