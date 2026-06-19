#include "BiomeGenerator.h"
#include "Vec4.h"

namespace WorldMaker {

    // ConditionGroup
    
    void ConditionGroup::addCondition(std::function<bool(const double &value)> condition) {;
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
            distanceVec[i] = 0.0;
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
}