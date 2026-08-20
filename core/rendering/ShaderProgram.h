#pragma once

#include <iostream>

#include <string>
#include <unordered_map>
#include "MeshMaterial.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "GL/glew.h"
#include <memory>

namespace WorldMaker
{
    class Material;

	enum ShaderProgramType
	{
		noise = 0,
		terrain = 1,
		model = 2,
		baking = 3,
	};

	class ShaderProgram
	{
	public:
		static ShaderProgram* s_boundShader;
		// Los paths son relativos a la carpeta WorldMaker
		ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);
		~ShaderProgram();

		void bind();
		void unbind() const;

		// Set Uniforms
		void setUniform1i(const std::string& name, int value);
		void setUniform2i(const std::string& name, int v0, int v1);
		void setUniform1f(const std::string& name, float value);
		void setUniform2f(const std::string& name, float v0, float v1);
		void setUniform3f(const std::string& name, float v0, float v1, float v2);
		void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
		void setUniformMat4f(const std::string& name, glm::mat4 matrix);
		void setUniformTextureSlots(const std::string& name);
		void loadTexture2DArray(unsigned int texture2DArray);
		void loadMeshMaterial(MeshMaterial* mat);
		void updateCameraMatrices();
		unsigned int maxTexturesCapacity() { return  maxTextureSlots; }
		unsigned int instanceId() const { return m_instanceId; }
		unsigned int rendererID() const { return m_glName; }

	private:
		unsigned int compileShader(unsigned int type, const std::string& source);
		unsigned int createProgram(const std::string& vertexShader, const std::string& fragmentShader);
		int getUniformLocation(const std::string& name);
		unsigned int m_glName;
		unsigned int m_instanceId;
		unsigned int static s_idInstanceCount;
		std::unordered_map<std::string, int> m_UniformLocationCache;
		std::string m_vFilePath;
		std::string m_fFilePath;
		static GLint maxTextureSlots;
	};

	using ShaderProgramSPtr = std::shared_ptr<ShaderProgram>;
}
