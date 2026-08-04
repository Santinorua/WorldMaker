#pragma once

#include "ChunkGeneration.h"

#define IMGUI_INPUT(var, type) (ImGui::InputScalar("##" #var, type, &var))

namespace WorldMaker {

namespace ui {

void begin();
void end();

/* Returns `true` if a redraw is requested or if chunks are regenerated */
bool GenerationWindow(int &chunk_size, int &world_width, uint64_t &seed, int& render_distance, ChunkGeneration::ChunkArray &chunks);
void BiomesWindow();

}

}
