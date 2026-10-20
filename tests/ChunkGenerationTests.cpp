#include "ChunkGeneration.h"
#include <chrono>
#include <cmath>
#include <iostream>
#include <set>
#include <stdexcept>
#include <thread>
#include <tuple>

using namespace WorldMaker;
using namespace WorldMaker::ChunkGeneration;
using namespace std::chrono_literals;

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

void testUpdatedProceduralGeneration() {
    Biome stateful;
    stateful.m_modifiers.emplace_back([offset = 0.0](const double& height) mutable {
        return height + ++offset;
    });
    require(stateful.applyModifiers(2) == 3 && stateful.applyModifiers(2) == 3,
            "Modifier callable state leaked between samples");
    // Local biomes deliberately differ from the editable global configuration.
    Biome low;
    low.name = "Low";
    low.biomeColor = {1, 0, 0, 1};
    Biome high;
    high.name = "High";
    high.biomeColor = {0, 1, 0, 1};
    double params[4] = {0.5, 0, 0, 0};
    for (int i = 0; i < 4; ++i) {
        low.setIdealCondition(static_cast<BiomeDeterminators>(i), params[i]);
        high.setIdealCondition(static_cast<BiomeDeterminators>(i), params[i]);
    }
    low.m_modifiers.emplace_back([](const double& height) { return height * 0.3; });
    high.m_modifiers.emplace_back([](const double& height) { return height * 2; });
    std::vector<Biome> biomes = {low, high};
    WorldGenerator generator(150, 42);
    std::vector<BiomeWeight> weights = {{0, 0.25}, {1, 0.75}};
    const auto modifiedHeight = generator.getHeight(0.8, 0, -0.5, 0, 1, weights, biomes);
    require(std::abs(modifiedHeight - 1.26) < 1e-9, "Biome modifier chains were not blended independently");

    biomes[0].m_modifiers = {Modifier([](const double&) { return 0.1; })};
    biomes[1].m_modifiers = {Modifier([](const double&) { return 0.5; })};
    biomes[0].m_features = {0};
    Feature feature;
    feature.modelPath = "test/model.glb";
    feature.radius = {1, 1, 1};
    feature.probability = {1, 1, 1};
    std::vector<Feature> features = {feature};
    const FractalNoise flatNoise(1, 0, 42, 1, 2, 0.5);
    generator.m_erosion = flatNoise;
    generator.m_continentalness = flatNoise;
    generator.m_temperature = flatNoise;
    generator.m_humidity = flatNoise;
    generator.m_rivers = flatNoise;
    generator.m_base = flatNoise;

    auto vertex = generator.getVertex(0, 0, biomes, features);
    require(std::abs(vertex.m_position.y - 45) < 1e-9, "New biome modifiers did not affect vertex height");
    require(vertex.m_color.x == 0.5 && vertex.m_color.y == 0.5 && vertex.m_color.z == 0,
            "Biome colors were not blended with height weights");
    require(vertex.m_featureId == 1, "Land features were not sampled from the captured configuration");

    // The UI supports partial feature drafts and orders valid entries by priority.
    Feature missingRadius = feature;
    missingRadius.radius.clear();
    Feature missingProbability = feature;
    missingProbability.probability.clear();
    Feature disabled = feature;
    disabled.probability[0] = 0;
    Feature invalidRadius = feature;
    invalidRadius.radius[0] = 0;
    Feature missingModel = feature;
    missingModel.modelPath.clear();
    auto featureDrafts = std::vector<Feature>{missingRadius, missingProbability, disabled,
                                             invalidRadius, missingModel, feature, feature};
    auto editedBiomes = biomes;
    editedBiomes[0].m_features = {-1, 100, 0, 1, 2, 3, 4, 5, 6};
    require(generator.getVertex(0, 0, editedBiomes, featureDrafts).m_featureId == 6,
            "Invalid feature drafts were used or first matching feature lost priority");
    editedBiomes[0].m_features = {6, 5};
    require(generator.getVertex(0, 0, editedBiomes, featureDrafts).m_featureId == 7,
            "Feature order from the UI was ignored");

    auto threeLandBiomes = biomes;
    Biome third = biomes[0];
    third.name = "Plateau";
    third.biomeColor = {0, 0, 1, 1};
    third.m_modifiers = {Modifier([](const double&) { return 0.9; })};
    threeLandBiomes.push_back(third);
    auto blended = BiomeGenerator::getBiomeWeights(params, threeLandBiomes);
    require(blended.size() == 3, "Nearby biomes were dropped from the new blend");
    for (const auto& weight : blended)
        require(std::abs(weight.weight - 1.0 / 3.0) < 1e-9, "Biome weights were not normalized");
    vertex = generator.getVertex(0, 0, threeLandBiomes, features);
    require(std::abs(vertex.m_position.y - 75) < 1e-9 && std::abs(vertex.m_color.z - 1.0 / 3.0) < 1e-9,
            "Three-biome height/color transition was lost");

    Biome river = third;
    river.name = "Rivers";
    river.m_modifiers = {Modifier([](const double&) { return -0.2; })};
    biomes.push_back(river);
    blended = BiomeGenerator::getBiomeWeights(params, biomes);
    require(blended.size() == 2, "Rivers were selected by climate instead of river noise");
    auto shoulderWeights = blended;
    auto shoulderHeight = generator.getHeight(0.5, 0, 0, 0.02, 0, shoulderWeights, biomes);
    require(std::abs(shoulderHeight - 0.05) < 1e-9, "River bank transition changed");
    auto edgeWeights = blended;
    require(std::abs(generator.getHeight(0.5, 0, 0, 0.04, 0, edgeWeights, biomes) - 0.3) < 1e-9,
            "River edge did not meet the surrounding terrain");
    auto dryWeights = blended;
    require(std::abs(generator.getHeight(0.5, 0, -0.5, 0, 0, dryWeights, biomes) - 0.3) < 1e-9,
            "River humidity condition was lost");
    vertex = generator.getVertex(0, 0, biomes, features);
    require(std::abs(vertex.m_position.y + 30) < 1e-9 && vertex.m_color.z == 1,
            "River carving or river color was lost");
    require(vertex.m_featureId == 0, "Submerged river features bypassed the new height condition");
}

ChunkData waitForResult(ChunkWorkerPool& pool) {
    const auto deadline = std::chrono::steady_clock::now() + 15s;
    ChunkData data;
    while (!pool.tryPop(data)) {
        require(std::chrono::steady_clock::now() < deadline, "Timed out waiting for a chunk");
        std::this_thread::sleep_for(1ms);
    }
    return data;
}

void compare(const ChunkData& a, const ChunkData& b) {
    require(a.vertices.size() == b.vertices.size(), "Vertex count changed");
    require(a.indices == b.indices, "Indices changed");
    require(a.tallest == b.tallest && a.lowest == b.lowest, "Bounds changed");
    for (size_t i = 0; i < a.vertices.size(); ++i)
        require(static_cast<std::vector<double>>(a.vertices[i]) == static_cast<std::vector<double>>(b.vertices[i]),
                "Parallel generation differs from serial generation");
    require(a.models.size() == b.models.size(), "Feature count changed");
    for (size_t i = 0; i < a.models.size(); ++i) {
        require(a.models[i].path == b.models[i].path && a.models[i].position == b.models[i].position &&
                a.models[i].rotation == b.models[i].rotation && a.models[i].scale == b.models[i].scale,
                "Feature placement changed");
    }
}

int main() {
    try {
        // Deliberately no GL context: workers must produce only CPU data.
        BiomeGenerator::addDefaultBiomes();
        FeatureManager::addDefaultFeatures();
        require(BiomeGenerator::getBiomeId("Rivers") >= 0, "The updated default river biome is missing");
        testUpdatedProceduralGeneration();
        WorldGenerator generator(150, 42);
        auto settings = std::make_shared<GenerationSettings>(generator);
        for (const auto& point : std::vector<glm::ivec2>{{0, 0}, {64, 128}, {800, 500}}) {
            auto direct = generator.getVertex(point.x, point.y);
            auto captured = generator.getVertex(point.x, point.y, settings->biomes, settings->features);
            require(static_cast<std::vector<double>>(direct) == static_cast<std::vector<double>>(captured) &&
                    direct.m_featureId == captured.m_featureId, "Captured sampling differs from the new public generator");
        }
        // Editable modifier parameters must also be captured, including operation order.
        Biome edited;
        edited.name = "Edited";
        edited.biomeColor = {1, 0, 0, 1};
        edited.m_modifiers = {Modifier(Modifier::Type::Absolute, 2, 1),
                              Modifier(Modifier::Type::Affine, 3, -1)};
        BiomeGenerator::m_biomes = {edited};
        auto editedSettings = std::make_shared<GenerationSettings>(generator);
        BiomeGenerator::m_biomes[0].m_modifiers[0].factor = 99;
        BiomeGenerator::m_biomes[0].moveModifierDown(0);
        require(editedSettings->biomes[0].applyModifiers(-2) == 14,
                "UI modifier edits changed a captured generation configuration");
        BiomeGenerator::m_biomes[0].m_modifiers = {Modifier(Modifier::Type::Affine, 0, 0.2)};
        auto updatedSettings = std::make_shared<GenerationSettings>(generator);
        const std::atomic_bool running = false;
        ChunkData updatedMesh;
        require(GenerateChunkData(*updatedSettings, {{0, 0}, 4}, updatedMesh, running),
                "Generation with edited UI modifiers failed");
        for (const auto& vertex : updatedMesh.vertices)
            require(std::abs(vertex.m_position.y - 30) < 1e-9,
                    "Regenerated terrain did not apply UI modifier changes");
        ChunkWorkerPool pool(3);
        std::vector<ChunkRequest> requests;
        for (int lod = 0; lod <= ChunkRenderUnit::MAX_LOD; ++lod)
            for (int x = 0; x < 4; ++x) requests.push_back({{x, 2}, lod});
        // Duplicate requests must not result in duplicate jobs.
        requests.push_back(requests.front());
        pool.request(requests, settings);
        pool.request(requests, settings);

        // Simulate UI edits while workers are sampling their captured world.
        generator = WorldGenerator(300, 9876);
        BiomeGenerator::m_biomes.clear();
        FeatureManager::m_features.clear();
        ChunkRenderUnit::s_chunkRes = 128;
        std::set<std::tuple<int, int, int>> seen;
        for (int i = 0; i < 20; ++i) {
            auto result = waitForResult(pool);
            require(seen.emplace(result.request.position.x, result.request.position.y, result.request.lod).second,
                    "Duplicate completion");
            ChunkData serial;
            require(GenerateChunkData(*settings, result.request, serial, running), "Serial generation cancelled");
            compare(result, serial);
            int side = (settings->resolution >> result.request.lod) + 1;
            require(result.vertices.size() == size_t(side * side), "Wrong LOD resolution");
            require(result.indices.size() == size_t((side - 1) * (side - 1) * 6), "Wrong triangle count");
            for (auto index : result.indices) require(index < result.vertices.size(), "Invalid triangle index");
            auto sampler = settings->generator;
            auto first = sampler.getVertex(result.request.position.x * settings->scale,
                result.request.position.y * settings->scale, settings->biomes, settings->features);
            require(result.vertices.front().m_position.y == first.m_position.y, "Terrain height changed");
        }
        ChunkData extra;
        require(!pool.tryPop(extra), "Unexpected extra result");

        // Reset must invalidate both running and already completed chunks, even
        // when the replacement uses the exact same coordinate and LOD.
        auto heavy = std::make_shared<GenerationSettings>(*settings);
        heavy->resolution = 1024;
        pool.request({{{0, 0}, 0}, {{1, 0}, 0}, {{2, 0}, 0}}, heavy);
        std::this_thread::sleep_for(5ms);
        pool.reset();
        auto replacement = std::make_shared<GenerationSettings>(*settings);
        replacement->resolution = 32;
        replacement->generator = generator;
        pool.request({{{0, 0}, 0}}, replacement);
        auto result = waitForResult(pool);
        ChunkData expected;
        GenerateChunkData(*replacement, {{0, 0}, 0}, expected, running);
        compare(result, expected);

        // Moving far away replaces pending/finished work without waiting for it.
        pool.request({{{1, 0}, 0}, {{2, 0}, 0}}, heavy);
        pool.request({{{50, 50}, 4}}, settings);
        result = waitForResult(pool);
        require(result.request.position == glm::ivec2(50, 50), "Obsolete camera request escaped cancellation");

        // Fill the ready queue, then reset it and verify that workers wake up.
        pool.request(requests, settings);
        std::this_thread::sleep_for(100ms);
        pool.reset();
        require(!pool.tryPop(extra), "Reset retained completed work");
        pool.request({{{3, 3}, 4}}, settings);
        require(waitForResult(pool).request.position == glm::ivec2(3, 3), "Pool did not resume after reset");

        const std::atomic_bool cancelled = true;
        require(!GenerateChunkData(*heavy, {{0, 0}, 0}, extra, cancelled), "Cancelled job still completed");
        pool.request({{{0, 0}, 0}}, heavy);
        pool.shutdown();
        pool.shutdown(); // Safe during explicit shutdown followed by destruction.
        require(!pool.tryPop(extra), "Shutdown retained completed work");

        require(GetChunkPos({0, 0, 0}) == glm::ivec2(0, 0), "Origin maps to wrong chunk");
        require(GetChunkPos({64, 0, 128}) == glm::ivec2(1, 2), "Boundary maps to wrong chunk");
        require(GetChunkPos({-0.5f, 0, -64}) == glm::ivec2(-1, -1), "Negative coordinate maps to wrong chunk");
        std::cout << "Chunk generation concurrency tests passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
