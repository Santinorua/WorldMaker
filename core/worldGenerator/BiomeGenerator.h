#pragma once
#include <vector>
#include <functional>
#include <algorithm>
#include <cmath>
#include "Texture2D.h"
#include "Features.h"
#include "Vec4.h"

namespace WorldMaker {

    enum class BiomeDeterminators {
        Erosion,
        Continentalness,
        Temperature,
        Humidity
    };

    class ConditionGroup {
        std::vector<std::function<bool(const double &value)>> m_conditions;
        bool m_isAnd = false;

        void addCondition(std::function<bool(const double &value)> condition);
        void setIsAnd(bool isAnd);
        bool evaluate(const double &value);
        void clearConditions();
        void removeCondition(int index);

        ConditionGroup() = default;
    };

    class Modifier {
        std::function<double(const double &value)> m_modifierFunction;

    public:
        enum class Type { Affine, NegativeScale, PositiveScale, Clamp, Absolute, Custom };
        Type type = Type::Affine;
        double factor = 1.0;
        double offset = 0.0;
        double minimum = -1.0;
        double maximum = 1.0;

        Modifier(Type type = Type::Affine, double factor = 1.0, double offset = 0.0)
            : type(type), factor(factor), offset(offset) {}
        double getValue(const double &value) const {
            switch (type) {
                case Type::Affine: return value * factor + offset;
                case Type::NegativeScale: return (value < 0 ? value * factor : value) + offset;
                case Type::PositiveScale: return (value > 0 ? value * factor : value) + offset;
                case Type::Clamp: return std::clamp(value, std::min(minimum, maximum), std::max(minimum, maximum));
                case Type::Absolute: return std::abs(value) * factor + offset;
                case Type::Custom: return m_modifierFunction ? m_modifierFunction(value) : value;
            }
            return value;
        }
        Modifier(std::function<double(const double &value)> modifierFunction)
            : m_modifierFunction(modifierFunction), type(Type::Custom) {}
    };

    // template<typename T>
    class Biome {
        double m_idealConditions[4] =  {-2.0, -2.0, -2.0, -2.0};
        std::vector<ConditionGroup> m_conditionGroups;

    public:
        std::string name;
        Vec4 biomeColor;
        std::vector<int> m_features;
        std::vector<Modifier> m_modifiers;

        void setIdealCondition(BiomeDeterminators determinator, double value) {
            m_idealConditions[(int)determinator] = value;
        }

        double getIdealCondition(BiomeDeterminators determinator) {
            return m_idealConditions[(int)determinator];
        }

        void removeIdealCondition(BiomeDeterminators determinator) {
            m_idealConditions[(int)determinator] = -2.0;
        }

        void addFeature(int featureId) {
            m_features.push_back(featureId);
        }

        void removeFeature(int featureId) {
            for (int i = 0; i < m_features.size(); i++) {
                if (m_features[i] == featureId) {
                    m_features.erase(m_features.begin() + i);
                    break;
                }
            }
        }



        double idealDistance(double params[4]);
        void addConditionGroup(const ConditionGroup &conditionGroup);
        void removeConditionGroup(const int index);
        void moveModifierUp(int idx);
        void moveModifierDown(int idx);
        double applyModifiers(const double &value);

    };


    struct BiomeWeight {
        int biomeId;
        double weight;
    };

    class BiomeGenerator {
    public:
        static std::vector<Biome> m_biomes;
        static Biome getBiome(double params[4]);
        // Normalized weights for all nearby biomes; the dominant biome is first.
        static std::vector<BiomeWeight> getBiomeWeights(double params[4]);
        static void addBiome(const Biome &biome);
        static void removeBiome(int index);
        static int getBiomeId(const std::string &name);
        static void addDefaultBiomes();
    };
}
