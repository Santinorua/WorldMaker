#pragma once

#include <ChunkRenderUnit.h>
#include <ChunkGeneration.h>

namespace WorldMaker {

struct Brush;

using DrawCallback = void (*)(const Brush&, std::vector<MeshVertex>&, glm::vec3 center);

struct Brush {
	float radius = 24;
	float strength = 0.3;
	float hardness = 0.5;

	float smoothFalloff(float distance) const noexcept;
	float weight(float distance) const noexcept;
};

void DrawBrush(const Brush& brush, std::vector<MeshVertex>& vertices, glm::vec3 center);

struct Ray {
	glm::vec3 from;
	// Normalized
	glm::vec3 direction;
};

// Returns the colision position, a vector of length 0 means no colision happened
// Takes a normalized vector for `direction`
glm::vec3 RaycastChunk(const ChunkRenderUnit& chunk, const Ray& ray);

glm::vec3 ScreenRaycast(const ChunkRenderUnit& chunk, glm::vec2 mouse_pos, glm::uvec2 screen_size);

}
