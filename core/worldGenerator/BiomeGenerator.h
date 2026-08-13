#pragma once
#include <vector>
#include <functional>
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

        double getValue(const double &value) {
            return m_modifierFunction(value);
        }
    };

    // template<typename T>
    class Biome {
        double m_idealConditions[4] =  {-2.0, -2.0, -2.0, -2.0};
        std::vector<ConditionGroup> m_conditionGroups;
        std::vector<Feature> m_features;
        std::vector<double> m_featureProbabilities;
        std::vector<Texture2D> m_featureTextures;
        double m_featureProbability = 0.1;
        std::vector<Modifier> m_modifiers;

    public:
        std::string name;
        Vec4 biomeColor;

        void setIdealCondition(BiomeDeterminators determinator, double value) {
            m_idealConditions[(int)determinator] = value;
        }

        double getIdealCondition(BiomeDeterminators determinator) {
            return m_idealConditions[(int)determinator];
        }

        void removeIdealCondition(BiomeDeterminators determinator) {
            m_idealConditions[(int)determinator] = -2.0;
        }
        double idealDistance(double params[4]);
        void addConditionGroup(const ConditionGroup &conditionGroup);
        void removeConditionGroup(int index);
        void addFeature(const Feature &feature, double probability, const Texture2D &texture);
        void removeFeature(int index);
        void moveModifierUp(int idx);
        void moveModifierDown(int idx);

    };


    class BiomeGenerator {
    public:
        static std::vector<Biome> m_biomes;
        static Biome getBiome(double params[4]);
        static void addBiome(const Biome &biome);
        static void removeBiome(int index);
        static void addDefaultBiomes();
    };
}
