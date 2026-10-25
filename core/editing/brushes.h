#pragma once

#include <ChunkRenderUnit.h>
#include <ChunkGeneration.h>

namespace WorldMaker {

struct Brush;

using DrawCallback = void (*)(const Brush& brush, std::vector<MeshVertex>&);

struct Brush {
	float radius = 0.5;
	float strength = 0.3;
	float hardness = 0.5;

	DrawCallback draw_callback = empty_callback;

	static float SmoothFalloff() noexcept;

private:
	static void empty_callback(const Brush&, std::vector<MeshVertex>&) {}
};

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
