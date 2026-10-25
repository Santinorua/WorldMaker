#pragma once

#include <ChunkRenderUnit.h>

struct Brush;

using DrawCallback = void (*)(const Brush& brush, std::vector<WorldMaker::MeshVertex>&);

struct Brush {
	float radius = 0.5;
	float strength = 0.3;
	float hardness = 0.5;

	DrawCallback draw_callback = empty_callback;

	static float SmoothFalloff() noexcept;

private:
	static void empty_callback(const Brush&, std::vector<WorldMaker::MeshVertex>&) {}
};
