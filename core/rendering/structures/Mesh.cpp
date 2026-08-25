#include "Mesh.h"
#include "TerrainMaterial.h"
#include "ResourceManager.h"
#include "ResourceManager.h"
#include "Vertex.h"
#include <memory>

namespace WorldMaker
{
	unsigned int Mesh::s_idCount = 0;

	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, MeshMaterialSPtr material)
		: m_material(material), m_id(s_idCount++)
	{
	    pushData(vertices, indices);
		submitData();
	}

	Mesh::~Mesh()
	{
	}


	void Mesh::submitData()
	{
		if (m_vertices->currentSize() > 0)
		{
			if (!m_uploaded)
			{
				m_vertices->submitData();
			}
		}
		if (m_indices->currentSize() > 0)
		{
			if (!m_uploaded)
			{
				m_indices->submitData();
			}
		}

		if (!m_uploaded && (m_vertices->currentSize() > 0 || m_indices->currentSize() > 0))
		{
			m_uploaded = true;
		}
	}

	void Mesh::bindBuffersBase()
	{
		m_vertices->bindBufferBase(SSBOType::vertices);
		m_indices->bindBufferBase(SSBOType::indices);
	}

	void Mesh::pushData(const std::vector<Vertex>& vertices, std::vector<unsigned int> indices)
	{
		for (unsigned int i = 0; i < indices.size() ; i++)
		{
			indices[i] += m_indexCount;
		}
		m_indexCount += vertices.size();

		m_vertices->pushData(vertices);

        m_indices->pushData(indices);
	}
}
