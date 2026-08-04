#include "ui.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include "CoreGenerator.h"
#include "Camera.h"
#include "ChunkRenderUnit.h"
#include "BiomeGenerator.h"

#include <iostream>

namespace WorldMaker {

namespace ui {

static bool show_biomes = false;
static Biome *biome_edit = nullptr;

void begin() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void end() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool GenerationWindow(int &chunk_size, int &world_width, uint64_t &seed, int& render_distance, ChunkGeneration::ChunkArray &chunks) {
	bool redraw = false;
	ImGui::Begin("Generation");
	ImGui::Text("Mesh Size: ");
	IMGUI_INPUT(chunk_size, ImGuiDataType_S32);

	ImGui::Text("Mesh size: ");
	IMGUI_INPUT(world_width, ImGuiDataType_S32);

	ImGui::Text("seed:");
	IMGUI_INPUT(seed, ImGuiDataType_U64);

	if (ImGui::Button("Biomes")) {
		show_biomes = !show_biomes;
		std::cout << "show_biomes = " << show_biomes << '\n';
	}

	if (ImGui::Button("Generate mesh")) {
		redraw = true;
		ChunkRenderUnit::s_chunkSide = chunk_size;
		ChunkRenderUnit::s_chunkSide = chunk_size;

		WorldGenerator generator(150,seed);

		for (auto &chunk : chunks) {
			delete chunk.second;
		}
		chunks.clear();
		ChunkGeneration::RegenerateChunks(chunks, generator, render_distance, Camera::Position(), true);
	}
	ImGui::End();
	return redraw;
}

void BiomeWindow(Biome *biome_ptr) {
	Biome &biome = *biome_ptr;

	static Biome *last_biome = nullptr;

	static double erosion = 0;
	static double continentalness = 0;
	static double temperature = 0;
	static double humidity = 0;
	static glm::vec4 color = {0.5, 0.5, 0.5, 1.0};

	if (last_biome != biome_ptr) {
		color = {biome.biomeColor.x, biome.biomeColor.y, biome.biomeColor.z, biome.biomeColor.w};

		erosion = biome.getIdealCondition(BiomeDeterminators::Erosion);
		continentalness = biome.getIdealCondition(BiomeDeterminators::Continentalness);
		temperature = biome.getIdealCondition(BiomeDeterminators::Temperature);
		humidity = biome.getIdealCondition(BiomeDeterminators::Humidity);

		last_biome = biome_ptr;
	}


	ImGui::Begin(biome.name.c_str());

	ImGui::InputDouble("Erosion: ", &erosion, 0, 0, "%.3f");
	ImGui::InputDouble("Continentalness: ", &continentalness, 0, 0, "%.3f");
	ImGui::InputDouble("Temperature: ", &temperature, 0, 0, "%.3f");
	ImGui::InputDouble("Humidity: ", &humidity, 0, 0, "%.3f");

	ImGui::ColorPicker4("Color", (float*)&color);

	if (ImGui::Button("Update Biome")) {
		biome.setIdealCondition(BiomeDeterminators::Erosion, erosion);
		biome.setIdealCondition(BiomeDeterminators::Continentalness, continentalness);
		biome.setIdealCondition(BiomeDeterminators::Temperature, temperature);
		biome.setIdealCondition(BiomeDeterminators::Humidity, humidity);

		biome.biomeColor.x = color.x;
		biome.biomeColor.y = color.y;
		biome.biomeColor.z = color.z;
		biome.biomeColor.w = color.w;
	}

	if (ImGui::Button("Close")) {
		biome_edit = nullptr;
	}

	ImGui::End();
}

void BiomesWindow() {
	if (!show_biomes) return;

	int biome_idx = 0;
	ImGui::Begin("Biomes");
	for (auto& biome : BiomeGenerator::m_biomes) {
		ImGui::Text("%s", biome.name.c_str());

		ImGui::SameLine();

		ImGui::PushID(biome.name.c_str());
		if (ImGui::Button("Edit")) {
			biome_edit = &biome;
		}
		ImGui::PopID();

		biome_idx++;
	}
	ImGui::End();

	if (biome_edit) {
		BiomeWindow(biome_edit);
	}
}

}

}
