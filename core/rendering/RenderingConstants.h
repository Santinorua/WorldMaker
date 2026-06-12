#pragma once
#include "glm/glm.hpp"

namespace WorldMaker
{
	static const unsigned int maxVertexAtribs = (3 + 4 + 1 + 2 + 1); // Position, Color, MaterialIndex, TexCoords, TexID
	static const unsigned int maxVertexSize = sizeof(double) * maxVertexAtribs; // 3 position, 4 color, 2 tex coords, 1 tex id
	static const unsigned int maxQuadCount = 10;
	static const unsigned int maxVertexCount = maxQuadCount * 1000;
	static const unsigned int maxIndexCount = maxQuadCount * 1000;
	static const unsigned int maxMaterials = 1024;
	static const unsigned int maxTextures = 512;
	static const unsigned int terrainTexturesWidth = 512;
	static const unsigned int terrainTexturesHeight = 512;
	static const unsigned int mat4Size = sizeof(glm::mat4);
	static const unsigned int maxMat4Size = maxVertexCount * mat4Size;
	static const unsigned int maxTextureCountPerType = 3000;
	static const unsigned int ssboAmplifier = 10; // When trying to add data to a ssbo and there is not enough space for more, change its max size
	static const unsigned int ssboOversizeMultiplier = 50; // When trying to add data to a ssbo and this data has a bigger size than the max capacity of the ssbo, change its max size
}
