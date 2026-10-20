#include "CoreGenerator.h"
#include <cassert>
#include <cmath>
#include <iostream>

using namespace WorldMaker;

static void near(double actual, double expected) {
    assert(std::abs(actual - expected) < 1e-12);
}

int main() {
    BiomeGenerator::addDefaultBiomes();
    FeatureManager::addDefaultFeatures();

    // Editable defaults must preserve the previous terrain functions.
    for (double value : {-1.0, -0.25, 0.0, 0.25, 1.0}) {
        auto height = [&](const char *name) {
            return BiomeGenerator::m_biomes[BiomeGenerator::getBiomeId(name)].applyModifiers(value);
        };
        near(height("Plains"), value * 0.3);
        near(height("Desert"), value * 0.6);
        near(height("Forest"), value);
        near(height("Hills"), (value < 0 ? value * -0.1 : value) + 0.01);
        near(height("Mountain"), (value < 0 ? value * -0.3 : value) + 0.6);
        near(height("Oceans"), (value > 0 ? value * -0.1 : value) * 0.2 - 0.6);
        near(height("Rivers"), (value > 0 ? value * -0.2 : value) - 0.2);
    }

    Biome chain;
    chain.moveModifierDown(0); // Empty list must be safe.
    chain.m_modifiers.emplace_back(Modifier::Type::Affine, 2, 0);
    chain.m_modifiers.emplace_back(Modifier::Type::Affine, 1, 3);
    near(chain.applyModifiers(1), 5);
    chain.moveModifierDown(0);
    near(chain.applyModifiers(1), 8);
    chain.moveModifierUp(1);
    near(chain.applyModifiers(1), 5);
    chain.m_modifiers.erase(chain.m_modifiers.begin());
    near(chain.applyModifiers(1), 4);
    Modifier clamp(Modifier::Type::Clamp);
    clamp.minimum = 2;
    clamp.maximum = -2;
    near(clamp.getValue(3), 2);
    near(Modifier(Modifier::Type::Absolute, 2, 1).getValue(-3), 7);

    Feature extra;
    extra.name = "Second";
    extra.modelPath = "core/rendering/assets/models/tree.glb";
    FeatureManager::addFeature(extra);
    auto &features = FeatureManager::m_features;
    const auto count = BiomeGenerator::m_biomes.size();
    assert(features[0].radius.size() == count && features[1].probability.size() == count);
    BiomeGenerator::m_biomes[0].addFeature(1);
    FeatureManager::removeFeature(0);
    assert(features.size() == 1);
    assert(BiomeGenerator::m_biomes[0].m_features == std::vector<int>{0});
    assert(BiomeGenerator::m_biomes[2].m_features.empty());

    Biome added;
    added.name = "Added";
    BiomeGenerator::addBiome(added);
    assert(features[0].radius.size() == count + 1);
    features[0].radius[2] = 37;
    features[0].probability[2] = 0.75;
    BiomeGenerator::removeBiome(1);
    assert(features[0].radius.size() == count);
    assert(features[0].radius[1] == 37);
    near(features[0].probability[1], 0.75);

    // Two assigned features: a failed later feature must not erase the first.
    features.clear();
    BiomeGenerator::m_biomes.clear();
    added.name = "Only";
    added.biomeColor = Vec4(0, 1, 0, 1);
    added.m_modifiers = {Modifier(Modifier::Type::Affine, 0, 1)};
    BiomeGenerator::addBiome(added);
    extra.radius = {1};
    extra.probability = {1};
    FeatureManager::addFeature(extra);
    extra.probability = {0};
    FeatureManager::addFeature(extra);
    BiomeGenerator::m_biomes[0].m_features = {0, 1};
    WorldGenerator generator(150, 42);
    assert(generator.getVertex(0, 0).m_featureId == 1);
    features[0].radius[0] = 0;
    assert(generator.getVertex(0, 0).m_featureId == 0);
    features[0].radius[0] = 1;
    features[0].probability[0] = 0;
    features[1].probability[0] = 1;
    assert(generator.getVertex(0, 0).m_featureId == 2);
    FeatureManager::removeFeature(0);
    assert(generator.getVertex(0, 0).m_featureId == 1);
    FeatureManager::removeFeature(0);
    assert(generator.getVertex(0, 0).m_featureId == 0);
    BiomeGenerator::removeBiome(0);
    assert(BiomeGenerator::m_biomes.size() == 1);
    std::cout << "PASS: editable terrain defaults, modifier ordering, feature/biome references and feature generation\n";
}
