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
	unsigned int render_distance;
};

namespace ui {

void init();
void begin();
void end();

void DockSpace(bool &quit, const ChunkGeneration::ChunkArray &chunks);

void DebugWindow(const unsigned int &render_distance, const ChunkGeneration::ChunkArray &chunks);

/* Returns `true` if a redraw is requested or if chunks are regenerated */
bool GenerationWindow(int &chunk_size, uint64_t &seed, const int& render_distance, ChunkGeneration::ChunkArray &chunks, WorldGenerator &generator, ChunkGeneration::ChunkStreamer& streamer);
void BiomesWindow();
void FeaturesWindow();

bool PreferencesWindow(Preferences &preferences);

}

}
