#pragma once
#include <vector>
#include <functional>
#include "Texture2D.h"
#include "Features.h"

namespace WorldMaker {
    // template<typename T>
    class ConditionGroup {
        std::vector<std::function<bool(const double &value)>> m_conditions;
        bool m_isAnd = false;

        void addCondition(std::function<bool(const double &value)> condition) {;
            m_conditions.push_back(condition);
        }

        void setIsAnd(bool isAnd) {
            m_isAnd = isAnd;
        }

        bool evaluate(const double &value) {
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

        void clearConditions() {
            m_conditions.clear();
        }

        void removeCondition(int index) {
            m_conditions.erase(m_conditions.begin() + index);
        }

        ConditionGroup() = default;

        ~ConditionGroup() {
            m_conditions.clear();
        }
    };

    class Modifier {
        std::function<double(const double &value)> m_modifierFunction;

        double getValue(const double &value) {
            return m_modifierFunction(value);
        }
    };

    // template<typename T>
    class Biome {
        std::vector<ConditionGroup> m_conditionGroups;
        std::vector<Feature> m_features;
        std::vector<double> m_featureProbabilities;
        std::vector<Texture2D> m_featureTextures;
        double m_featureProbability = 0.1;
        std::vector<Modifier> m_modifiers;

    };

    class BiomeGenerator {

    };
}