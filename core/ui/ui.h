#pragma once

#include "ChunkGeneration.h"

#define IMGUI_INPUT(var, type) (ImGui::InputScalar("##" #var, type, &var))

namespace WorldMaker {

void GenerationWindow(int &chunk_size, int &world_width, uint64_t &seed, int& render_distance, ChunkGeneration::ChunkArray &chunks);

}
