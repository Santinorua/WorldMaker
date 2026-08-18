#include "BiomeGenerator.h"
#include "Vec4.h"

namespace WorldMaker {

    std::vector<Biome> BiomeGenerator::m_biomes = {};

    // ConditionGroup
    
    void ConditionGroup::addCondition(std::function<bool(const double &value)> condition) {
        m_conditions.push_back(condition);
    }

    void ConditionGroup::setIsAnd(bool isAnd) {
        m_isAnd = isAnd;
    }

    bool ConditionGroup::evaluate(const double &value) {
        if (m_isAnd) {
            for (const std::function<bool(const double &value)> &condition : m_conditions) {
                if (!condition(value)) {
                    return false;
                }
            }
            return true;
        }

        for (const std::function<bool(const double &value)> &condition : m_conditions) {
            if (condition(value)) {
                return true;
            }
        }
        return false;
    }

    void ConditionGroup::clearConditions() {
        m_conditions.clear();
    }

    void ConditionGroup::removeCondition(int index) {
        m_conditions.erase(m_conditions.begin() + index);
    }




    // Biome

    double Biome::idealDistance(double params[4]) {
        double distance = 0.0;
        double distanceVec[4];
        for (int i = 0; i < 4; i++) {
            distanceVec[i] = 0.5;
            if (m_idealConditions[i] != -2.0) {
                distanceVec[i] = params[i] - m_idealConditions[i];
            }
        }
        distance = Vec4(distanceVec[0], distanceVec[1], distanceVec[2], distanceVec[3]).Magnitude();
        return distance;
    }

    void Biome::addConditionGroup(const ConditionGroup &conditionGroup) {
        m_conditionGroups.push_back(conditionGroup);
    }

    void Biome::removeConditionGroup(int index) {
        m_conditionGroups.erase(m_conditionGroups.begin() + index);
    }

    void Biome::addFeature(const Feature &feature, double probability, const Texture2D &texture) {
        m_features.push_back(feature);
        m_featureProbabilities.push_back(probability);
        m_featureTextures.push_back(texture);
    }

    void Biome::removeFeature(int index) {
        m_features.erase(m_features.begin() + index);
        m_featureProbabilities.erase(m_featureProbabilities.begin() + index);
        m_featureTextures.erase(m_featureTextures.begin() + index);
    }


    void Biome::moveModifierUp(int idx) {
        if (idx <= 0 || idx >= m_modifiers.size()) return;
        std::swap(m_modifiers[idx], m_modifiers[idx - 1]);
    }

    void Biome::moveModifierDown(int idx) {
        if (idx < 0 || idx >= m_modifiers.size() - 1) return;
        std::swap(m_modifiers[idx], m_modifiers[idx + 1]);
    }

    Biome BiomeGenerator::getBiome(double params[4]) {
        assert(m_biomes.size() > 0);
        std::vector<double> distances;
        distances.reserve(m_biomes.size());
        double bestDistance = std::numeric_limits<double>::max();
        int bestBiome = 0;
        for (int i = 0; i < m_biomes.size(); i++) {
            bool conditionsMet = true;
            // Comprobar condiciones
            if (conditionsMet) {
                double distance = m_biomes[i].idealDistance(params);
                distances.push_back(distance);
                if (distance < bestDistance) {
                    bestDistance = distance;
                    bestBiome = i;
                }
            }
        }
        return m_biomes[bestBiome];
    }

    void BiomeGenerator::addBiome(const Biome &biome) {
        m_biomes.push_back(biome);
    }

    void BiomeGenerator::removeBiome(int index) {
        m_biomes.erase(m_biomes.begin() + index);
    }

    void BiomeGenerator::addDefaultBiomes() {
        // Plains

        Biome plains = Biome();
        plains.name = "Plains";
        plains.setIdealCondition(BiomeDeterminators::Continentalness, -0.4);
        plains.setIdealCondition(BiomeDeterminators::Erosion, 0);
        plains.setIdealCondition(BiomeDeterminators::Temperature, 0.5);
        plains.setIdealCondition(BiomeDeterminators::Humidity,0.2);
        plains.biomeColor = Vec4(0.0, 1.0, 0.0, 1.0);

        addBiome(plains);

        Biome desert = Biome();
        desert.name = "Desert";
        desert.setIdealCondition(BiomeDeterminators::Continentalness, 0.5);
        desert.setIdealCondition(BiomeDeterminators::Erosion, 0.5);
        desert.setIdealCondition(BiomeDeterminators::Temperature, 1);
        desert.setIdealCondition(BiomeDeterminators::Humidity,-1);
        desert.biomeColor = Vec4(1.0, 1.0, 0.0, 1.0);

        addBiome(desert);

        // Biome mountain = Biome();
        // mountain.name = "Mountain";
        // mountain.setIdealCondition(BiomeDeterminators::Continentalness, 0.8);
        // mountain.setIdealCondition(BiomeDeterminators::Erosion, 1);
        // mountain.setIdealCondition(BiomeDeterminators::Temperature, -2);
        // mountain.setIdealCondition(BiomeDeterminators::Humidity,-2);
        // mountain.biomeColor = Vec4(0.42, 0.91, 0.42, 1.0);
        //
        // addBiome(mountain);

        Biome forest = Biome();
        forest.name = "Forest";
        forest.setIdealCondition(BiomeDeterminators::Continentalness, 0);
        forest.setIdealCondition(BiomeDeterminators::Erosion, 0.1);
        forest.setIdealCondition(BiomeDeterminators::Temperature, 0.5);
        forest.setIdealCondition(BiomeDeterminators::Humidity,0.6);
        forest.biomeColor = Vec4(0.0, 0.57, 0.0, 1.0);

        addBiome(forest);
    }


}