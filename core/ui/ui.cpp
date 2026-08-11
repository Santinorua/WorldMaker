#include "ui.h"
#include "colors.h"

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

#define _HEX(r, g, b, a) { (float)(r)/255.0, (float)(g)/255.0, (float)(b)/255.0, (float)(a)/255.0 }
#define HEX(hex) _HEX((((uint64_t)hex) >> 24) & 0xff, (((uint64_t)hex) >> 16) & 0xff, (((uint64_t)hex) >> 8) & 0xff, (uint64_t)hex & 0xff)

void init() {
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(Renderer::GetWindow(), true);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();

	ImGuiStyle &style = ImGui::GetStyle();

	style.Colors[ImGuiCol_WindowBg] = HEX(Colors::Gruvbox::bg0);

	style.Colors[ImGuiCol_ResizeGrip] = HEX(Colors::Gruvbox::bg1);
	style.Colors[ImGuiCol_ResizeGripHovered] = HEX(Colors::Gruvbox::grey1);
	style.Colors[ImGuiCol_ResizeGripActive] = HEX(Colors::Gruvbox::grey2);

	style.Colors[ImGuiCol_TitleBg] = HEX(Colors::Gruvbox::bg1);
	style.Colors[ImGuiCol_TitleBgActive] = HEX(Colors::Gruvbox::bg2); // TODO: Find better color

	style.Colors[ImGuiCol_Button] = HEX(Colors::Gruvbox::bg2);
	style.Colors[ImGuiCol_ButtonHovered] = HEX(Colors::Gruvbox::blue);
	style.Colors[ImGuiCol_ButtonActive] = HEX(Colors::Gruvbox::aqua);

	style.Colors[ImGuiCol_FrameBg] = HEX(Colors::Gruvbox::bg4);

	style.FrameRounding = 3.0;
	style.WindowRounding = 5.0;

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void begin() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void end() {
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DockSpace() {
	int w, h;
	glfwGetWindowSize(Renderer::GetWindow(), &w, &h);

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(w, h));

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus	|
		ImGuiWindowFlags_NoDocking	|
		ImGuiWindowFlags_NoTitleBar	|
		ImGuiWindowFlags_NoResize	|
		ImGuiWindowFlags_NoMove		|
		ImGuiWindowFlags_NoCollapse	|
		ImGuiWindowFlags_MenuBar	|
		ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	ImGui::Begin("Main", NULL, window_flags);

	ImGui::PopStyleVar();

	ImGui::DockSpace(ImGui::GetID("Dockspace"), ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);
}

void DebugWindow(int &render_distance, const ChunkGeneration::ChunkArray &chunks) {
	ImGui::Begin("Debug");

	int w, h;
	glfwGetWindowSize(Renderer::GetWindow(), &w, &h);
	ImGui::Text("%dx%d", w, h);

	ImGui::Text("FPS: %.1f",
			ImGui::GetIO().Framerate);

	IMGUI_INPUT(render_distance, ImGuiDataType_S32);

	auto chunk_pos = ChunkGeneration::GetChunkPos(Camera::Position());
	ImGui::Text("Chunk Pos: (%d, %d)", chunk_pos.x, chunk_pos.y);

	auto x_generation_range = ChunkGeneration::GetGenerationRange(chunk_pos.x, render_distance);
	ImGui::Text("X Gen. Range: [%d; %d]", x_generation_range.x, x_generation_range.y);

	auto y_generation_range = ChunkGeneration::GetGenerationRange(chunk_pos.y, render_distance);
	ImGui::Text("X Gen. Range: [%d; %d]", y_generation_range.x, y_generation_range.y);

	ImGui::Text("Chunks Loaded: %lu", chunks.size());

	ImGui::End();
}

bool GenerationWindow(int &chunk_size, int &world_width, uint64_t &seed, int& render_distance, ChunkGeneration::ChunkArray &chunks, WorldGenerator &generator) {
	bool redraw = false;
	
	ImGui::Begin("Generation");

	ImGui::PushItemWidth(100);

	ImGui::Text("Mesh Size: ");
	IMGUI_INPUT(chunk_size, ImGuiDataType_S32);

	ImGui::Text("Mesh size: ");
	IMGUI_INPUT(world_width, ImGuiDataType_S32);

	ImGui::Text("seed:");
	IMGUI_INPUT(seed, ImGuiDataType_U64);

	ImGui::PopItemWidth();

	if (ImGui::Button("Biomes")) {
		show_biomes = !show_biomes;
	}

	if (ImGui::Button("Generate mesh")) {
		redraw = true;
		ChunkRenderUnit::s_chunkSide = chunk_size;
		ChunkRenderUnit::s_chunkSide = chunk_size;

		generator = WorldGenerator(150, seed);

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

	ImGui::Text("Erosion:");
	ImGui::SameLine();
	ImGui::InputDouble("##Erosion: ", &erosion, 0, 0, "%.3f");

	ImGui::Text("Continentalness:");
	ImGui::SameLine();
	ImGui::InputDouble("##Continentalness: ", &continentalness, 0, 0, "%.3f");

	ImGui::Text("Temperature:");
	ImGui::SameLine();
	ImGui::InputDouble("##Temperature: ", &temperature, 0, 0, "%.3f");

	ImGui::Text("Humidity:");
	ImGui::SameLine();
	ImGui::InputDouble("##Humidity: ", &humidity, 0, 0, "%.3f");

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
