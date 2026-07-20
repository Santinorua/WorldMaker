#include "Model.h"
#include "RenderingAssets.h"
#include "ResourceManager.h"
#include "Vec3.h"
#include "FileFunctions.h"
#include "assimp/material.h"
#include "assimp/matrix4x4.h"

#include <memory>
namespace WorldMaker
{
	unsigned int Model::s_idCount = 0;

	Model::~Model()
	{
        ResourceManager::RemoveModelIfExpired(m_relativePath);
	}

	Model::Model(const std::string& path)
		: m_relativePath(path), m_globalPath(globalizePath(path)), m_instanceId(s_idCount++)
	{
		loadModel(m_globalPath);
	}
	void Model::loadModel(const std::string& path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(
            path,
            aiProcess_Triangulate |
            aiProcess_FlipWindingOrder |
            aiProcess_JoinIdenticalVertices |
            aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_ImproveCacheLocality
        );
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << "\n";
			return;
		}
		aiMatrix4x4 identityMatrix;
		processNode(scene->mRootNode, scene, identityMatrix);
	}
	void Model::processNode(aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransform)
	{
	    aiMatrix4x4 globalPos = parentTransform * node->mTransformation;
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_meshes.push_back(processMesh(mesh, scene, globalPos));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene, globalPos);
		}
	}


	MeshSPtr Model::processMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4 globalPos)
	{
		ModelMaterialSPtr sharedMeshMaterial = nullptr;

		bool materialFound = false;
		for (auto& pair : m_localToGlobalMaterials)
		{
			if (pair.first == mesh->mMaterialIndex)
			{
				sharedMeshMaterial = pair.second.lock();
				ResourceManager::GetModelMaterial(sharedMeshMaterial->id());
				materialFound = true;
				break;
			}
		}
		if (!materialFound)
		{
    		Texture2DSPtr diffuse;
    		Texture2DSPtr specular;

    		if (mesh->mMaterialIndex >= 0)
    		{
    			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    			aiString texAiPath;

                diffuse = getTexture2D(aiTextureType_DIFFUSE, material, texAiPath, *scene);
                specular = getTexture2D(aiTextureType_SPECULAR, material, texAiPath, *scene);
                sharedMeshMaterial = ResourceManager::CreateModelMaterial(diffuse, specular);
                m_localToGlobalMaterials.emplace(mesh->mMaterialIndex, sharedMeshMaterial);
    		}
    		else std::cout << "No materials\n";
		}

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		for (unsigned int  i = 0; i < mesh->mNumVertices; i++)
		{

			Vertex vertex;
			vertex.m_materialLayer = sharedMeshMaterial->id();
			glm::vec3 vector;

			aiVector3D pos = globalPos * mesh->mVertices[i];
			vertex.m_position = { pos.x, pos.y, pos.z };

			if (mesh->mTextureCoords[0])
			{
				vector.x = mesh->mTextureCoords[0][i].x;
				vector.y = mesh->mTextureCoords[0][i].y;
				vertex.m_uv = Vec2(vector.x, vector.y);
			}
			else vertex.m_uv = {0.0f, 0.0f};

			aiMatrix3x3 normalMatrix{globalPos};
            normalMatrix.Inverse();
            normalMatrix.Transpose();

            aiVector3D normal = normalMatrix * mesh->mNormals[i];
            normal.Normalize();

            vertex.m_normal = { normal.x, normal.y, normal.z };

			vertices.push_back(vertex);
		}


		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) indices.push_back(face.mIndices[j]);
		}


		return std::make_shared<Mesh>(vertices, indices, sharedMeshMaterial);
	}

	Texture2DSPtr Model::getTexture2D(aiTextureType texType, aiMaterial* material, aiString& texAiPath, const aiScene& scene)
	{
        if (material->GetTexture(texType, 0, &texAiPath) == AI_SUCCESS)
        {
            std::string assimpPath = texAiPath.C_Str();
            if (assimpPath.empty())
            {
          		std::cout << "Assimp path empty when getting texture from model\n";
          		ASSERT(false);
            }
            else if (assimpPath[0] == '*')
            {
                const aiTexture* aiTex = scene.GetEmbeddedTexture(texAiPath.C_Str());
                return ResourceManager::LoadTexture(m_relativePath+assimpPath, aiTex);
            }
            else
            {
          		std::string absoluteModelPath = std::filesystem::path(m_globalPath).parent_path().string();
                return ResourceManager::LoadTexture(relativizePath(absoluteModelPath) + "/" + assimpPath);
            }
        }
        return ResourceManager::LoadTexture(specularTexDefaultPath);
	}
}
