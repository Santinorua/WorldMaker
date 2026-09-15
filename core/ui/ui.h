#pragma once

#include "ChunkGeneration.h"

#define IMGUI_INPUT(var, type) (ImGui::InputScalar("##" #var, type, &var))

namespace WorldMaker {

enum class PolygonMode {
	Fill = 0,
	Wireframe = 1,
};

struct Preferences {
	bool frustrum_culling_enabled = true;
	PolygonMode polygon_mode = PolygonMode::Fill;
	PolygonMode _current_polygon_mode = PolygonMode::Fill;

	float *camera_speed;
};

namespace ui {

void init();
void begin();
void end();

void DockSpace(bool &quit);

void DebugWindow(int &render_distance, const ChunkGeneration::ChunkArray &chunks);

/* Returns `true` if a redraw is requested or if chunks are regenerated */
bool GenerationWindow(int &chunk_size, uint64_t &seed, int& render_distance, ChunkGeneration::ChunkArray &chunks, WorldGenerator &generator);
void BiomesWindow();

bool PreferencesWindow(Preferences &preferences);

}

}
