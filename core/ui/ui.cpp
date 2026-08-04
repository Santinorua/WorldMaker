#include "ui.h"

#include "imgui.h"
#include "CoreGenerator.h"
#include "Camera.h"
#include "ChunkRenderUnit.h"


namespace WorldMaker {

static bool show_biomes = false;

void GenerationWindow(int &chunk_size, int &world_width, uint64_t &seed, int& render_distance, ChunkGeneration::ChunkArray &chunks) {
	if (ImGui::Begin("Generation"))	{
		ImGui::Text("Mesh Size: ");
		IMGUI_INPUT(chunk_size, ImGuiDataType_S32);

		ImGui::Text("Mesh size: ");
		IMGUI_INPUT(world_width, ImGuiDataType_S32);

		ImGui::Text("seed:");
		IMGUI_INPUT(seed, ImGuiDataType_U64);

		if (ImGui::Button("Biomes")) {
			show_biomes = !show_biomes;
		}

		if (ImGui::Button("Generate mesh")) {
			ChunkRenderUnit::s_chunkSide = chunk_size;
			ChunkRenderUnit::s_chunkSide = chunk_size;

			WorldGenerator generator(150,seed);

			ChunkGeneration::RegenerateChunks(chunks, generator, render_distance, Camera::Position());
		}
	}
	ImGui::End();
}

void BiomeWindow() {

}

}
