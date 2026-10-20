#include "ChunkGeneration.h"
#include <chrono>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <thread>

using namespace WorldMaker;
using namespace WorldMaker::ChunkGeneration;
using namespace std::chrono_literals;

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

int main() {
    try {
        require(glfwInit(), "GLFW initialization failed");
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        auto* window = glfwCreateWindow(64, 64, "Chunk streaming test", nullptr, nullptr);
        require(window != nullptr, "OpenGL 4.4 context unavailable");
        glfwMakeContextCurrent(window);
        require(glewInit() == GLEW_OK, "GLEW initialization failed");
        GLClearError();
        BiomeGenerator::addDefaultBiomes();
        WorldGenerator generator(150, 42);
        ChunkArray chunks;
        ChunkStreamer streamer;

        const auto settle = [&](glm::vec3 camera, int distance, size_t count) {
            const auto deadline = std::chrono::steady_clock::now() + 15s;
            while (true) {
                auto before = chunks.size();
                streamer.update(chunks, generator, distance, camera);
                require(chunks.size() <= before + 2, "Too many GPU uploads in one frame");
                auto center = GetChunkPos(camera);
                bool done = chunks.size() == count;
                for (const auto& [position, chunk] : chunks) {
                    require(position.x >= center.x - distance && position.x < center.x + distance &&
                            position.y >= center.y - distance && position.y < center.y + distance,
                            "Out-of-range chunk was retained");
                    auto delta = glm::abs(position - center);
                    float ratio = float(std::max(delta.x, delta.y)) / distance;
                    int lod = ratio > 0.8f ? 4 : ratio > 0.6f ? 3 : ratio > 0.45f ? 2 : 0;
                    done &= chunk->m_current_lod == lod;
                }
                if (done) break;
                require(std::chrono::steady_clock::now() < deadline, "Streaming did not finish with stationary camera");
                std::this_thread::sleep_for(1ms);
            }
            for (auto& [position, chunk] : chunks) {
                auto& buffer = *chunk->m_vertices[chunk->m_current_lod];
                std::vector<MeshVertex> downloaded(buffer.m_data.size());
                buffer.bind();
                glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
                    downloaded.size() * sizeof(MeshVertex), downloaded.data());
                for (size_t i = 0; i < downloaded.size(); ++i)
                    require(static_cast<std::vector<double>>(downloaded[i]) ==
                            static_cast<std::vector<double>>(buffer.m_data[i]), "GPU mesh differs from generated mesh");
            }
        };

        settle({256, 0, 256}, 2, 16);
        // Moving must retain the active world until Generate mesh is requested,
        // even if the editor changes or removes biome/feature definitions.
        const auto activeBiomes = BiomeGenerator::m_biomes;
        BiomeGenerator::m_biomes.clear();
        settle({384, 0, 256}, 2, 16); // Exercise unloading, loading and LOD refinement.
        for (auto& [position, chunk] : chunks) {
            auto& vertices = chunk->m_vertices[chunk->m_current_lod]->m_data;
            require(vertices.front().m_position.y ==
                    generator.getVertex(position.x * 64, position.y * 64, activeBiomes, {}).m_position.y,
                    "Moving the camera applied uncommitted UI edits");
        }
        BiomeGenerator::m_biomes = activeBiomes;
        settle({384, 0, 256}, 1, 4);  // Stationary camera, changed draw distance.
        settle({384, 0, 256}, 3, 36);
        generator = WorldGenerator(150, 99);
        ChunkRenderUnit::s_chunkRes = 32;
        streamer.update(chunks, generator, 1, {384, 0, 256}, true);
        settle({384, 0, 256}, 1, 4);
        for (auto& [position, chunk] : chunks) {
            auto& vertices = chunk->m_vertices[chunk->m_current_lod]->m_data;
            require(vertices.front().m_position.y == generator.getVertex(position.x * 64, position.y * 64).m_position.y,
                    "Old seed survived regeneration");
            int side = (32 >> chunk->m_current_lod) + 1;
            require(vertices.size() == size_t(side * side), "Old resolution survived regeneration");
        }
        // Generate mesh must apply modifier edits even with the same seed/resolution.
        for (auto& biome : BiomeGenerator::m_biomes)
            biome.m_modifiers = {Modifier(Modifier::Type::Affine, 0, 0.2)};
        streamer.update(chunks, generator, 1, {384, 0, 256}, true);
        settle({384, 0, 256}, 1, 4);
        for (auto& [position, chunk] : chunks)
            for (const auto& vertex : chunk->m_vertices[chunk->m_current_lod]->m_data)
                require(std::abs(vertex.m_position.y - 30) < 1e-9,
                        "Generate mesh did not apply edited UI modifiers");
        // Exercise the real feature/model upload path and its cache as well.
        FeatureManager::addDefaultFeatures();
        for (auto& biome : BiomeGenerator::m_biomes) {
            biome.m_features = {0};
            // Keep this asset-upload fixture above water, including river cells.
            biome.m_modifiers = {Modifier([](const double&) { return 0.1; })};
        }
        FeatureManager::m_features[0].radius.assign(BiomeGenerator::m_biomes.size(), 1);
        FeatureManager::m_features[0].probability.assign(BiomeGenerator::m_biomes.size(), 1.0);
        ChunkRenderUnit::s_chunkRes = 16;
        streamer.update(chunks, generator, 1, {0, 0, 0}, true);
        settle({0, 0, 0}, 1, 1);
        require(!chunks.front().second->m_models.m_modelInstancesSSBO.empty(), "Features were not uploaded");
        auto& modelEntry = chunks.front().second->m_models.m_modelInstancesSSBO.begin()->second;
        std::weak_ptr<Model> cachedModel = modelEntry.first;
        const auto modelId = modelEntry.first->instanceId();
        auto& matrices = *modelEntry.second;
        std::vector<glm::mat4> downloadedMatrices(matrices.m_data.size());
        matrices.bind();
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
            downloadedMatrices.size() * sizeof(glm::mat4), downloadedMatrices.data());
        require(downloadedMatrices == matrices.m_data, "Batched model transforms were not uploaded");
        settle({-256, 0, -256}, 1, 0);
        require(!cachedModel.expired(), "Model was evicted when its last chunk unloaded");
        settle({0, 0, 0}, 1, 1);
        require(chunks.front().second->m_models.m_modelInstancesSSBO.begin()->second.first->instanceId() == modelId,
                "Re-entering a chunk reloaded its model");

        // Start a new batch and shut down immediately, before releasing GL resources.
        streamer.update(chunks, generator, 4, {1024, 0, 1024});
        streamer.shutdown();
        for (auto& chunk : chunks) delete chunk.second;
        chunks.clear();
        require(glGetError() == GL_NO_ERROR, "OpenGL error during chunk streaming");
        glfwDestroyWindow(window);
        glfwTerminate();
        std::cout << "Chunk streaming OpenGL tests passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
