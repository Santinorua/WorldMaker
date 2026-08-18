#include "Features.h"
#include <algorithm>

#include "BiomeGenerator.h"

namespace WorldMaker {

    std::vector<Feature> FeatureManager::m_features = {};

    void FeatureManager::addFeature(const Feature &feature) {
        m_features.push_back(feature);
    }

    void FeatureManager::removeFeature(const int index) {
        m_features.erase(m_features.begin() + index);
    }

    int FeatureManager::getFeatureId(const Feature &feature) {
        auto it = std::find_if(m_features.begin(), m_features.end(), [&feature](const Feature &f) {
            return f.name == feature.name;
        });
        if (it != m_features.end()) {
            return std::distance(m_features.begin(), it);
        }
        return -1; // Feature not found
    }

    void FeatureManager::addDefaultFeatures() {
        Feature tree;
        tree.name = "Tree";
        tree.modelPath = "core/rendering/assets/models/tree.glb";
        tree.scale = {0.1, 0.1f, 0.1f};
        tree.radius.push_back(20);
        tree.radius.push_back(20);
        tree.radius.push_back(20);
        tree.probability.push_back(0.05);
        tree.probability.push_back(0);
        tree.probability.push_back(0.85);

        m_features.push_back(tree);
        BiomeGenerator::m_biomes[BiomeGenerator::getBiomeId("Plains")].addFeature(getFeatureId(tree));
        BiomeGenerator::m_biomes[BiomeGenerator::getBiomeId("Forest")].addFeature(getFeatureId(tree));
    }

}
