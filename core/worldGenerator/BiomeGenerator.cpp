#include "BiomeGenerator.h"
#include "Interpolations.h"
#include "Vec4.h"

#include <algorithm>
#include <cassert>
#include <limits>

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
                // if (i == int(BiomeDeterminators::Erosion)) {
                //     distanceVec[i] *= 2;
                // }
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

    void Biome::moveModifierUp(int idx) {
        if (idx <= 0 || idx >= m_modifiers.size()) return;
        std::swap(m_modifiers[idx], m_modifiers[idx - 1]);
    }

    void Biome::moveModifierDown(int idx) {
        if (idx < 0 || idx + 1 >= m_modifiers.size()) return;
        std::swap(m_modifiers[idx], m_modifiers[idx + 1]);
    }

    double Biome::applyModifiers(const double &value) {
        double final = value;
        for (Modifier modifier : m_modifiers) {
            final = modifier.getValue(final);
        }
        return final;
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

    std::vector<BiomeWeight> BiomeGenerator::getBiomeWeights(double params[4]) {
        assert(m_biomes.size() > 0);
        std::vector<double> distances;
        distances.reserve(m_biomes.size());
        double bestDistance = std::numeric_limits<double>::max();
        int bestBiome = 0;
        for (int i = 0; i < m_biomes.size(); i++) {
            double distance = m_biomes[i].idealDistance(params);
            if (m_biomes[i].name == "Rivers") {
                distance = 50.0; // Rivers should not be selected as the best biome based on distance
            }
            distances.push_back(distance);
            if (distance < bestDistance) {
                bestDistance = distance;
                bestBiome = i;
            }
        }

        constexpr double biomeTransitionThreshold = 0.8;
        std::vector<BiomeWeight> weights;
        weights.reserve(m_biomes.size());
        weights.push_back({bestBiome, 1.0});
        double totalWeight = 1.0;
        for (int i = 0; i < m_biomes.size(); i++) {
            if (i == bestBiome) continue;

            // Coincident ideal conditions share the weight without dividing by zero.
            double relativeDistance = distances[i] == 0.0 ? 1.0 : bestDistance / distances[i];
            double transition = std::clamp((relativeDistance - biomeTransitionThreshold)
                                          / (1.0 - biomeTransitionThreshold), 0.0, 1.0);
            double blend = SmoothStep(0.5 * transition);
            if (blend == 0.0) continue;

            // Preserve the existing two-biome curve after normalization:
            // weight / (1 + weight) == blend. Include every nearby biome so a
            // change in second place cannot suddenly add or remove a modifier.
            double weight = blend / (1.0 - blend);
            weights.push_back({i, weight});
            totalWeight += weight;
        }
        for (BiomeWeight &biomeWeight : weights) {
            biomeWeight.weight /= totalWeight;
        }
        return weights;
    }

    void BiomeGenerator::addBiome(const Biome &biome) {
        m_biomes.push_back(biome);
        for (Feature &feature : FeatureManager::m_features) {
            feature.radius.resize(m_biomes.size(), 20);
            feature.probability.resize(m_biomes.size(), 0.05);
        }
    }

    void BiomeGenerator::removeBiome(int index) {
        if (index < 0 || index >= m_biomes.size() || m_biomes.size() <= 1) return;
        for (Feature &feature : FeatureManager::m_features) {
            if (index < feature.radius.size()) feature.radius.erase(feature.radius.begin() + index);
            if (index < feature.probability.size()) feature.probability.erase(feature.probability.begin() + index);
        }
        m_biomes.erase(m_biomes.begin() + index);
    }

    int BiomeGenerator::getBiomeId(const std::string &name) {
        for (int i = 0; i < m_biomes.size(); i++) {
            if (m_biomes[i].name == name) {
                return i;
            }
        }
        return -1; // Biome not found
    }


    void BiomeGenerator::addDefaultBiomes() {
        // Plains

        Biome plains = Biome();
        plains.name = "Plains";
        plains.setIdealCondition(BiomeDeterminators::Continentalness, -0.4);
        plains.setIdealCondition(BiomeDeterminators::Erosion, 0);
        plains.setIdealCondition(BiomeDeterminators::Temperature, 0.5);
        plains.setIdealCondition(BiomeDeterminators::Humidity,0.0);
        plains.biomeColor = Vec4(0.0, 1.0, 0.0, 1.0);
        plains.m_modifiers.push_back(Modifier(Modifier::Type::Affine, 0.3));

        addBiome(plains);

        Biome desert = Biome();
        desert.name = "Desert";
        desert.setIdealCondition(BiomeDeterminators::Continentalness, 0.4);
        desert.setIdealCondition(BiomeDeterminators::Erosion, 0.1);
        desert.setIdealCondition(BiomeDeterminators::Temperature, 0.8);
        desert.setIdealCondition(BiomeDeterminators::Humidity,-1);
        desert.biomeColor = Vec4(1.0, 1.0, 0.0, 1.0);
        desert.m_modifiers.push_back(Modifier(Modifier::Type::Affine, 0.6));

        addBiome(desert);

        Biome forest = Biome();
        forest.name = "Forest";
        forest.setIdealCondition(BiomeDeterminators::Continentalness, 0);
        forest.setIdealCondition(BiomeDeterminators::Erosion, 0.1);
        forest.setIdealCondition(BiomeDeterminators::Temperature, 0.5);
        forest.setIdealCondition(BiomeDeterminators::Humidity,0.6);
        forest.biomeColor = Vec4(0.0, 0.415, 0.0, 1.0);
        // forest.m_modifiers.push_back(Modifier([](const double &value) -> double {
            // return value * 4;
        // }));
        addBiome(forest);

        Biome hill = Biome();
        hill.name = "Hills";
        hill.setIdealCondition(BiomeDeterminators::Continentalness, 1.0);
        hill.setIdealCondition(BiomeDeterminators::Erosion, 0.7);
        hill.setIdealCondition(BiomeDeterminators::Temperature, -2);
        hill.setIdealCondition(BiomeDeterminators::Humidity,-2);
        hill.biomeColor = Vec4(0.0, 0.6, 0.0, 1.0);
        hill.m_modifiers.push_back(Modifier(Modifier::Type::NegativeScale, -0.1, 0.01));

        addBiome(hill);

        Biome mountain = Biome();
        mountain.name = "Mountain";
        mountain.setIdealCondition(BiomeDeterminators::Continentalness, 1.0);
        mountain.setIdealCondition(BiomeDeterminators::Erosion, 1.0);
        mountain.setIdealCondition(BiomeDeterminators::Temperature, -2);
        mountain.setIdealCondition(BiomeDeterminators::Humidity,-2);
        mountain.biomeColor = Vec4(0.3, 0.3, 0.3, 1.0);
        mountain.m_modifiers.push_back(Modifier(Modifier::Type::NegativeScale, -0.3, 0.6));

        addBiome(mountain);

        Biome ocean = Biome();
        ocean.name = "Oceans";
        ocean.setIdealCondition(BiomeDeterminators::Continentalness, -0.7);
        ocean.setIdealCondition(BiomeDeterminators::Erosion, 0.2);
        ocean.setIdealCondition(BiomeDeterminators::Temperature, -2);
        ocean.setIdealCondition(BiomeDeterminators::Humidity,0.0);
        ocean.biomeColor = Vec4(0.0, 0.0, 1.0, 1.0);
        ocean.m_modifiers.push_back(Modifier(Modifier::Type::PositiveScale, -0.1));
        ocean.m_modifiers.push_back(Modifier(Modifier::Type::Affine, 0.2, -0.6));
        addBiome(ocean);


        Biome river = Biome();
        river.name = "Rivers";
        river.setIdealCondition(BiomeDeterminators::Continentalness, -2);
        river.setIdealCondition(BiomeDeterminators::Erosion, -2);
        river.setIdealCondition(BiomeDeterminators::Temperature, -2);
        river.setIdealCondition(BiomeDeterminators::Humidity,-2);
        river.biomeColor = Vec4(0.0, 0.0, 1.0, 1.0);
        river.m_modifiers.push_back(Modifier(Modifier::Type::PositiveScale, -0.2, -0.2));
        addBiome(river);

    }


}
