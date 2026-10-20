#include "ui.h"
#include "colors.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include "CoreGenerator.h"
#include "Camera.h"
#include "ChunkRenderUnit.h"
#include "BiomeGenerator.h"
#include "WorldExporter.h"

#include "Input.h"

#include <cstdio>
#include <algorithm>
#include <cmath>
#include <cctype>
#include <filesystem>
#include <set>
#include "FileFunctions.h"

namespace WorldMaker {

namespace ui {

static int biome_edit = -1;
static bool reload_biome = false;
static bool show_features = false;
static int feature_edit = -2;
static Biome biome_draft;
static std::vector<bool> feature_enabled_biomes;

static struct {
	bool debug;
	bool generation;
	bool biomes;
	bool biome_edit;
	bool new_biome;
	bool preferences;
} open_windows = {
	.debug = true,
	.generation = true,
	.biomes = false,
	.biome_edit = false,
	.new_biome = false,
	.preferences = false,
};

static bool &show_biomes = open_windows.biomes;
static bool create_new_biome = false;

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

	style.Colors[ImGuiCol_TitleBg] = HEX(Colors::Gruvbox::bg0);
	style.Colors[ImGuiCol_TitleBgActive] = HEX(Colors::Gruvbox::bg1); // TODO: Find better color

	style.Colors[ImGuiCol_Tab] = HEX(Colors::Gruvbox::bg1);
	style.Colors[ImGuiCol_TabActive] = HEX(Colors::Gruvbox::bg2);
	style.Colors[ImGuiCol_TabHovered] = HEX(Colors::Gruvbox::grey1);

	style.Colors[ImGuiCol_TabUnfocused] = HEX(Colors::Gruvbox::bg1);
	style.Colors[ImGuiCol_TabUnfocusedActive] = HEX(Colors::Gruvbox::bg1);

	style.Colors[ImGuiCol_TabSelectedOverline] = HEX(Colors::Gruvbox::bg2);
	style.Colors[ImGuiCol_TabDimmedSelectedOverline] = HEX(Colors::Gruvbox::bg1);

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
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

static bool show_export = false;
static bool export_attempted = false;
static bool export_succeeded = false;
static size_t exported_chunk_count = 0;

static void ExportWindow(const ChunkGeneration::ChunkArray &chunks) {
	if (!show_export) return;
	using Coordinate = std::pair<int, int>;
	static std::set<Coordinate> selected;
	static bool initialized = false;
	std::set<Coordinate> loaded;
	for (const auto &[position, chunk] : chunks)
		if (chunk) loaded.emplace(position.x, position.y);
	if (!initialized && !loaded.empty()) {
		selected = loaded;
		initialized = true;
	}
	// Keep coordinates, never chunk pointers: chunks can be regenerated or unloaded.
	std::erase_if(selected, [&](const Coordinate &coordinate) { return !loaded.contains(coordinate); });

	ImGui::SetNextWindowSize(ImVec2(460, 520), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Export World", &show_export)) {
		ImGui::Text("Selected: %zu / %zu loaded chunks", selected.size(), loaded.size());
		ImGui::TextWrapped("Choose the chunks to include. Newly loaded chunks are not selected automatically.");
		if (ImGui::Button("Select All")) selected = loaded;
		ImGui::SameLine();
		if (ImGui::Button("Select None")) selected.clear();
		ImGui::SameLine();
		if (ImGui::Button("Invert Selection")) {
			for (const auto &coordinate : loaded) {
				if (!selected.erase(coordinate)) selected.insert(coordinate);
			}
		}
		if (ImGui::BeginTable("Chunks", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0, 240))) {
			ImGui::TableSetupColumn("Export");
			ImGui::TableSetupColumn("Chunk X");
			ImGui::TableSetupColumn("Chunk Z");
			ImGui::TableSetupScrollFreeze(0, 1);
			ImGui::TableHeadersRow();
			for (const auto &coordinate : loaded) {
				ImGui::PushID(coordinate.first);
				ImGui::PushID(coordinate.second);
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				bool included = selected.contains(coordinate);
				if (ImGui::Checkbox("##ExportChunk", &included)) {
					if (included) selected.insert(coordinate); else selected.erase(coordinate);
				}
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%d", coordinate.first);
				ImGui::TableSetColumnIndex(2);
				ImGui::Text("%d", coordinate.second);
				ImGui::PopID();
				ImGui::PopID();
			}
			ImGui::EndTable();
		}
		if (loaded.empty()) ImGui::TextUnformatted("No chunks loaded.");
		ImGui::TextWrapped("Exports selected terrain and models to world.glb. Replaces the previous export.");
		std::error_code error;
		auto destination = std::filesystem::current_path(error);
		if (!error) ImGui::TextWrapped("Destination: %s", (destination / "world.glb").string().c_str());
		ImGui::BeginDisabled(selected.empty());
		if (ImGui::Button("Export Selected (.glb)")) {
			ChunkGeneration::ChunkArray subset;
			for (const auto &entry : chunks)
				if (entry.second && selected.contains({entry.first.x, entry.first.y})) subset.push_back(entry);
			export_succeeded = WorldExporter::ExportWorld(subset);
			export_attempted = true;
			exported_chunk_count = subset.size();
		}
		ImGui::EndDisabled();
		if (export_attempted) {
			if (export_succeeded) ImGui::Text("Exported %zu chunks to world.glb", exported_chunk_count);
			else ImGui::TextUnformatted("Export failed. Check the console for details.");
		}
	}
	ImGui::End();
}

void DockSpace(bool &quit, const ChunkGeneration::ChunkArray &chunks) {
	if (!ImGui::GetIO().WantCaptureKeyboard && ImGui::IsKeyPressed(ImGuiKey_Space, false)) show_export = true;
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

	if (ImGui::BeginMenuBar()) {

		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Preferences...")) {
				open_windows.preferences = true;
			}
			if (ImGui::MenuItem("Exit")) {
				quit = true;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View")) {
			if (ImGui::MenuItem((open_windows.generation) ? "Generation *" : "Generation")) {
				open_windows.generation = !open_windows.generation;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Export")) {
			ImGui::Text("Loaded chunks: %zu", chunks.size());
			ImGui::Separator();
			if (ImGui::MenuItem("Choose chunks and export...", "Space")) show_export = true;
			ImGui::EndMenu();
		}

		if (export_attempted) {
			ImGui::TextUnformatted(export_succeeded ? "World exported: world.glb" : "World export failed. Check the console for details.");
		}

		ImGui::EndMenuBar();
	}

	ImGui::End();
	ExportWindow(chunks);
}

void DebugWindow(const unsigned int &render_distance, const ChunkGeneration::ChunkArray &chunks) {
	if (!open_windows.debug) return;

	ImGui::Begin("Debug", &open_windows.debug);

	int w, h;
	glfwGetWindowSize(Renderer::GetWindow(), &w, &h);
	ImGui::Text("%dx%d", w, h);

	ImGui::Text("FPS: %.1f",
			ImGui::GetIO().Framerate);

	auto chunk_pos = ChunkGeneration::GetChunkPos(Camera::Position());
	ImGui::Text("Chunk Pos: (%d, %d)", chunk_pos.x, chunk_pos.y);

	auto x_generation_range = ChunkGeneration::GetGenerationRange(chunk_pos.x, render_distance);
	ImGui::Text("X Gen. Range: [%d; %d]", x_generation_range.x, x_generation_range.y);

	auto y_generation_range = ChunkGeneration::GetGenerationRange(chunk_pos.y, render_distance);
	ImGui::Text("X Gen. Range: [%d; %d]", y_generation_range.x, y_generation_range.y);

	ImGui::Text("Chunks Loaded: %lu", chunks.size());

	ImGui::End();
}

bool GenerationWindow(int &chunk_size, uint64_t &seed, const int& render_distance, ChunkGeneration::ChunkArray &chunks, WorldGenerator &generator, ChunkGeneration::ChunkStreamer& streamer) {
	bool redraw = false;

	if (!open_windows.generation) return false;

	ImGui::Begin("Generation", &open_windows.generation);

	ImGui::PushItemWidth(100);

	ImGui::Text("Mesh Size: ");
	IMGUI_INPUT(chunk_size, ImGuiDataType_S32);

	ImGui::Text("seed:");
	IMGUI_INPUT(seed, ImGuiDataType_U64);

	ImGui::PopItemWidth();

	if (ImGui::Button("Biomes")) {
		show_biomes = !show_biomes;
	}

	ImGui::SameLine();
	if (ImGui::Button("Features")) show_features = !show_features;
	ImGui::TextWrapped("After editing biomes or features, click Generate mesh to apply changes.");

	if (ImGui::Button("Generate mesh")) {
		redraw = true;
		// Every LOD needs at least one cell and must cover the same chunk extent.
		chunk_size = std::max(16, chunk_size / 16 * 16);
		ChunkRenderUnit::s_chunkRes = chunk_size;

		generator = WorldGenerator(150, seed);

		streamer.update(chunks, generator, render_distance, Camera::Position(), true);
	}
	ImGui::End();
	return redraw;
}

static void ModifierControls(Biome &biome) {
	ImGui::TextWrapped("Modifiers run from top to bottom on terrain height.");
	const char *types[] = {"Scale + offset", "Scale negative heights", "Scale positive heights", "Clamp", "Absolute", "Custom"};
	for (int i = 0; i < static_cast<int>(biome.m_modifiers.size()); ++i) {
		ImGui::PushID(i);
		Modifier &modifier = biome.m_modifiers[i];
		ImGui::Separator();
		ImGui::Text("Modifier %d", i + 1);
		int type = static_cast<int>(modifier.type);
		if (ImGui::Combo("Operation", &type, types, modifier.type == Modifier::Type::Custom ? 6 : 5))
			modifier.type = static_cast<Modifier::Type>(type);
		if (modifier.type == Modifier::Type::Clamp) {
			ImGui::InputDouble("Minimum", &modifier.minimum, 0, 0, "%.3f");
			ImGui::InputDouble("Maximum", &modifier.maximum, 0, 0, "%.3f");
		} else if (modifier.type != Modifier::Type::Custom) {
			ImGui::InputDouble("Factor", &modifier.factor, 0, 0, "%.3f");
			ImGui::InputDouble("Offset", &modifier.offset, 0, 0, "%.3f");
		} else ImGui::TextWrapped("Custom function. Choose an operation to replace it.");
		int action = 0;
		ImGui::BeginDisabled(i == 0);
		if (ImGui::Button("Up")) action = 1;
		ImGui::EndDisabled();
		ImGui::SameLine();
		ImGui::BeginDisabled(i + 1 == biome.m_modifiers.size());
		if (ImGui::Button("Down")) action = 2;
		ImGui::EndDisabled();
		ImGui::SameLine();
		if (ImGui::Button("Remove")) action = 3;
		ImGui::PopID();
		if (action == 1) biome.moveModifierUp(i);
		if (action == 2) biome.moveModifierDown(i);
		if (action == 3) biome.m_modifiers.erase(biome.m_modifiers.begin() + i);
		if (action) break;
	}
	if (ImGui::Button("Add Modifier")) biome.m_modifiers.emplace_back();
}

void BiomeWindow() {
	if (!open_windows.biome_edit || biome_edit < 0 || biome_edit >= BiomeGenerator::m_biomes.size()) return;
	Biome &biome = BiomeGenerator::m_biomes[biome_edit];
	Biome &draft = biome_draft;
	if (reload_biome) {
		draft = biome;
		reload_biome = false;
	}
	ImGui::SetNextWindowSize(ImVec2(430, 610), ImGuiCond_FirstUseEver);
	std::string title = biome.name + "###biome_edit";
	if (ImGui::Begin(title.c_str(), &open_windows.biome_edit)) {
		const char *conditions[] = {"Erosion", "Continentalness", "Temperature", "Humidity"};
		for (int i = 0; i < 4; ++i) {
			double value = draft.getIdealCondition(static_cast<BiomeDeterminators>(i));
			if (ImGui::InputDouble(conditions[i], &value, 0, 0, "%.3f"))
				draft.setIdealCondition(static_cast<BiomeDeterminators>(i), value);
		}
		float color[] = {static_cast<float>(draft.biomeColor.x), static_cast<float>(draft.biomeColor.y), static_cast<float>(draft.biomeColor.z), static_cast<float>(draft.biomeColor.w)};
		if (ImGui::ColorEdit4("Color", color, ImGuiColorEditFlags_DisplayHex))
			draft.biomeColor = Vec4(color[0], color[1], color[2], color[3]);
		if (ImGui::CollapsingHeader("Modifiers", ImGuiTreeNodeFlags_DefaultOpen)) ModifierControls(draft);
		if (ImGui::CollapsingHeader("Features")) {
			ImGui::TextWrapped("Choose features for this biome. Model and distribution settings are in the Features window.");
			for (int i = 0; i < FeatureManager::m_features.size(); ++i) {
				ImGui::PushID(i);
				bool enabled = std::find(draft.m_features.begin(), draft.m_features.end(), i) != draft.m_features.end();
				if (ImGui::Checkbox(FeatureManager::m_features[i].name.c_str(), &enabled)) {
					if (enabled) draft.addFeature(i); else draft.removeFeature(i);
				}
				ImGui::PopID();
			}
			if (ImGui::Button("Manage Features")) show_features = true;
		}
		ImGui::Separator();
		bool valid = true;
		for (int i = 0; i < 4; ++i)
			valid = valid && std::isfinite(draft.getIdealCondition(static_cast<BiomeDeterminators>(i)));
		for (const Modifier &modifier : draft.m_modifiers)
			valid = valid && std::isfinite(modifier.factor) && std::isfinite(modifier.offset)
				&& std::isfinite(modifier.minimum) && std::isfinite(modifier.maximum);
		if (!valid) ImGui::TextUnformatted("Conditions and modifier parameters must be finite.");
		ImGui::BeginDisabled(!valid);
		if (ImGui::Button("Update Biome")) {
			biome = draft;
			if (feature_edit >= 0 && biome_edit < feature_enabled_biomes.size())
				feature_enabled_biomes[biome_edit] = std::find(biome.m_features.begin(), biome.m_features.end(), feature_edit) != biome.m_features.end();
		}
		ImGui::EndDisabled();
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) open_windows.biome_edit = false;
	}
	ImGui::End();
}

void NewBiomeWindow() {

	static Biome *biome = nullptr;

	static double erosion = 0;
	static double continentalness = 0;
	static double temperature = 0;
	static double humidity = 0;
	static glm::vec4 color = {0.5, 0.5, 0.5, 1.0};
	static char biome_name[64] = {0};

	if (!open_windows.new_biome) {
		return;
	}

	if (create_new_biome) {
		create_new_biome = false;

		delete biome;
		biome = new Biome();
		biome_name[0] = '\0';

		biome->name = "";
		color = {biome->biomeColor.x, biome->biomeColor.y, biome->biomeColor.z, 1};

		erosion = 0;
		continentalness = -1;
		temperature = -1;
		humidity = -1;

		Vec2 cursor_pos = Input::GetCursorPosPix();

		cursor_pos.x -= 45;
		cursor_pos.y -= 45;

		ImGui::SetNextWindowPos(ImVec2(cursor_pos.x, cursor_pos.y));
	}

	ImGui::SetNextWindowSize(ImVec2(250, 350));

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	ImGui::Begin("New Biome", &open_windows.new_biome, flags);

	ImGui::PushItemWidth(75);


	ImGui::Text("Name:");
	ImGui::SameLine();
	ImGui::InputText("##Name", biome_name, 64);

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

	ImGui::PopItemWidth();

	ImGui::PushItemWidth(175);

	ImGuiColorEditFlags color_picker_flags = ImGuiColorEditFlags_DisplayHex;

	ImGui::ColorPicker4("Color", (float*)&color, color_picker_flags);

	if (ImGui::Button("Create Biome")) {
		biome->setIdealCondition(BiomeDeterminators::Erosion, erosion);
		biome->setIdealCondition(BiomeDeterminators::Continentalness, continentalness);
		biome->setIdealCondition(BiomeDeterminators::Temperature, temperature);
		biome->setIdealCondition(BiomeDeterminators::Humidity, humidity);

		biome->biomeColor.x = color.x;
		biome->biomeColor.y = color.y;
		biome->biomeColor.z = color.z;
		biome->biomeColor.w = color.w;

		biome->name = biome_name;

		open_windows.biome_edit = false;
		feature_edit = -2;
		BiomeGenerator::addBiome(*biome);
		delete biome;
		biome = nullptr;
		open_windows.new_biome = false;
	}

	ImGui::PopItemWidth();

	ImGui::End();
}

void BiomesWindow() {
	if (!show_biomes) return;

	ImGui::Begin("Biomes", &open_windows.biomes);
	for (int biome_idx = 0; biome_idx < BiomeGenerator::m_biomes.size(); biome_idx++) {
		Biome &biome = BiomeGenerator::m_biomes[biome_idx];
		ImGui::Text("%s", biome.name.c_str());

		ImGui::SameLine(0, 10);

		ImGui::PushID(biome_idx);
		if (ImGui::Button("Edit")) {
			biome_edit = biome_idx;
			reload_biome = true;
			open_windows.biome_edit = true;
		}
		ImGui::PopID();

		ImGui::SameLine(0, 10);

		ImGui::PushID(biome_idx);
		if (ImGui::Button("Remove")) {
			ImGui::OpenPopup("Remove Biome");
		}

		if (ImGui::BeginPopup("Remove Biome")) {
			if (BiomeGenerator::m_biomes.size() <= 1) ImGui::TextUnformatted("At least one biome is required.");
			if (BiomeGenerator::m_biomes.size() > 1 && ImGui::Button("Remove")) {
				open_windows.biome_edit = false;
				biome_edit = -1;
				feature_edit = -2;
				BiomeGenerator::removeBiome(biome_idx--);
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::PopID();
	}

	if (ImGui::Button("Add Biome")) {
		open_windows.new_biome = true;
		create_new_biome = true;
	}

	ImGui::End();

	if (open_windows.biome_edit) {
		BiomeWindow();
	}

	if (open_windows.new_biome) {
		NewBiomeWindow();
	}
}

void FeaturesWindow() {
	if (!show_features) return;
	static Feature draft;
	static char name[128] = {};
	static char model_path[1024] = {};
	static glm::vec3 rotation_degrees(0);
	auto &enabled_biomes = feature_enabled_biomes;
	static std::vector<std::string> models;
	auto edit = [&](int index) {
		feature_edit = index;
		draft = index >= 0 ? FeatureManager::m_features[index] : Feature();
		draft.radius.resize(BiomeGenerator::m_biomes.size(), 20);
		draft.probability.resize(BiomeGenerator::m_biomes.size(), 0.05);
		snprintf(name, sizeof(name), "%s", draft.name.c_str());
		snprintf(model_path, sizeof(model_path), "%s", draft.modelPath.c_str());
		rotation_degrees = glm::degrees(glm::eulerAngles(draft.rotation));
		enabled_biomes.assign(BiomeGenerator::m_biomes.size(), false);
		for (int i = 0; i < enabled_biomes.size(); ++i) {
			const auto &ids = BiomeGenerator::m_biomes[i].m_features;
			enabled_biomes[i] = std::find(ids.begin(), ids.end(), index) != ids.end();
		}
		models.clear();
		std::error_code error;
		const std::filesystem::path root(globalizePath("core/rendering/assets/models"));
		for (std::filesystem::recursive_directory_iterator it(root, error), end; it != end && !error; it.increment(error)) {
			if (!it->is_regular_file(error)) continue;
			std::string extension = it->path().extension().string();
			std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return std::tolower(c); });
			if (extension == ".glb" || extension == ".gltf" || extension == ".obj" || extension == ".fbx")
				models.push_back(it->path().lexically_relative(std::filesystem::path(globalizePath(""))).generic_string());
		}
		std::sort(models.begin(), models.end());
	};
	ImGui::SetNextWindowSize(ImVec2(560, 650), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Features", &show_features)) { ImGui::End(); return; }
	for (int i = 0; i < FeatureManager::m_features.size(); ++i) {
		ImGui::PushID(i);
		ImGui::TextUnformatted(FeatureManager::m_features[i].name.c_str());
		ImGui::SameLine();
		if (ImGui::Button("Edit")) edit(i);
		ImGui::SameLine();
		if (ImGui::Button("Remove")) ImGui::OpenPopup("Remove Feature");
		bool removed = false;
		if (ImGui::BeginPopup("Remove Feature")) {
			ImGui::TextUnformatted("Remove this feature from all biomes?");
			if (ImGui::Button("Remove")) {
				FeatureManager::removeFeature(i);
				feature_edit = -2;
				if (open_windows.biome_edit) {
					auto &ids = biome_draft.m_features;
					ids.erase(std::remove(ids.begin(), ids.end(), i), ids.end());
					for (int &id : ids) if (id > i) --id;
				}
				removed = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
		ImGui::PopID();
		if (removed) break;
	}
	if (ImGui::Button("Add Feature")) edit(-1);
	if (feature_edit >= -1) {
		ImGui::Separator();
		ImGui::TextUnformatted(feature_edit == -1 ? "New Feature" : "Edit Feature");
		ImGui::InputText("Name", name, sizeof(name));
		if (ImGui::BeginCombo("Model", model_path[0] ? model_path : "Choose a model")) {
			for (const auto &model : models) {
				if (ImGui::Selectable(model.c_str(), model == model_path))
					snprintf(model_path, sizeof(model_path), "%s", model.c_str());
			}
			ImGui::EndCombo();
		}
		ImGui::InputText("Model path", model_path, sizeof(model_path));
		ImGui::TextWrapped("Model paths are relative to the project folder. Formats: GLB, GLTF, OBJ, FBX.");
		ImGui::DragFloat3("Scale (XYZ)", &draft.scale.x, 0.01f, 0.001f, 1000.0f, "%.3f");
		ImGui::DragFloat3("Rotation (degrees)", &rotation_degrees.x, 1.0f);
		ImGui::InputDouble("Height offset", &draft.y_offset, 0, 0, "%.3f");
		ImGui::Separator();
		ImGui::TextUnformatted("Distribution by biome");
		ImGui::TextWrapped("Radius is grid spacing; probability is the chance per grid cell (0 to 1). Features appear on land above water. List order sets priority when features overlap.");
		for (int i = 0; i < BiomeGenerator::m_biomes.size(); ++i) {
			ImGui::PushID(i);
			bool enabled = enabled_biomes[i];
			if (ImGui::Checkbox(BiomeGenerator::m_biomes[i].name.c_str(), &enabled)) enabled_biomes[i] = enabled;
			if (enabled) {
				ImGui::InputInt("Radius", &draft.radius[i]);
				draft.radius[i] = std::clamp(draft.radius[i], 1, 10000);
				ImGui::InputDouble("Probability", &draft.probability[i], 0.01, 0.1, "%.3f");
				draft.probability[i] = std::clamp(draft.probability[i], 0.0, 1.0);
			}
			ImGui::PopID();
		}
		bool valid = name[0] && model_path[0] && std::isfinite(draft.y_offset);
		for (int i = 0; i < 3; ++i)
			valid = valid && std::isfinite(draft.scale[i]) && draft.scale[i] > 0 && std::isfinite(rotation_degrees[i]);
		for (double probability : draft.probability) valid = valid && std::isfinite(probability);
		std::error_code error;
		valid = valid && std::filesystem::is_regular_file(globalizePath(model_path), error);
		for (int i = 0; i < FeatureManager::m_features.size(); ++i)
			if (i != feature_edit && FeatureManager::m_features[i].name == name) valid = false;
		if (!valid) ImGui::TextWrapped("Enter a unique name, an existing model path and finite values with positive scale.");
		ImGui::BeginDisabled(!valid);
		if (ImGui::Button(feature_edit == -1 ? "Create Feature" : "Update Feature")) {
			draft.name = name;
			draft.modelPath = model_path;
			draft.rotation = glm::quat(glm::radians(rotation_degrees));
			int id = feature_edit;
			if (id == -1) {
				id = static_cast<int>(FeatureManager::m_features.size());
				FeatureManager::addFeature(draft);
			} else FeatureManager::m_features[id] = draft;
			for (int i = 0; i < BiomeGenerator::m_biomes.size(); ++i) {
				Biome &biome = BiomeGenerator::m_biomes[i];
				const bool assigned = std::find(biome.m_features.begin(), biome.m_features.end(), id) != biome.m_features.end();
				if (enabled_biomes[i] && !assigned) biome.addFeature(id);
				if (!enabled_biomes[i]) biome.removeFeature(id);
			}
			if (open_windows.biome_edit) {
				const auto &ids = biome_draft.m_features;
				const bool assigned = std::find(ids.begin(), ids.end(), id) != ids.end();
				if (enabled_biomes[biome_edit] && !assigned) biome_draft.addFeature(id);
				if (!enabled_biomes[biome_edit]) biome_draft.removeFeature(id);
			}
			feature_edit = -2;
		}
		ImGui::EndDisabled();
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) feature_edit = -2;
	}
	ImGui::End();
}

bool PreferencesWindow(Preferences &preferences) {
	if (!open_windows.preferences) return false;

	ImGui::Begin("Preferences", &open_windows.preferences);

	ImGui::PushItemWidth(100);

	ImGui::LabelText("RenderingTitle", "Rendering");

	ImGui::Checkbox("Frustrum Culling", &preferences.frustrum_culling_enabled);
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
		ImGui::SetTooltip("WARNING: WIP, visual glitches may occur");
	}

	const std::array<const char*, 2> polygon_options = {"Fill", "Wireframe"};

	int selected = static_cast<int>(preferences._current_polygon_mode);
	if (ImGui::BeginCombo("Polygon Mode", polygon_options[selected])) {
		if (ImGui::Selectable("Fill")) {
			selected = 0;
			preferences.polygon_mode = PolygonMode::Fill;
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
				ImGui::SetTooltip("Default polygon mode");
			}
		}

		if (ImGui::Selectable("Wireframe")) {
			selected = 1;
			preferences.polygon_mode = PolygonMode::Wireframe;

			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
				ImGui::SetTooltip("Debug purposes mainly, renders only the lines between vertices");
			}
		}

		ImGui::EndCombo();
	}


	ImGui::LabelText("Misc. Title", "Misc.");

	ImGui::Text("Camera Speed: ");
	ImGui::SameLine();
	IMGUI_INPUT(*preferences.camera_speed, ImGuiDataType_Float);

	ImGui::Text("Render Distance: ");
	ImGui::SameLine();
	IMGUI_INPUT(preferences.render_distance, ImGuiDataType_U32);

	ImGui::PopItemWidth();

	ImGui::End();

	if (preferences._current_polygon_mode != preferences.polygon_mode) {
		preferences._current_polygon_mode = preferences.polygon_mode;
		switch (preferences._current_polygon_mode) {
			case PolygonMode::Wireframe:
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;
			case PolygonMode::Fill:
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				break;
		}
	}

	return true;
}

}

}
