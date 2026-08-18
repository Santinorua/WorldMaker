#pragma once
#include <vector>
#include <string>

#include "glm/vec3.hpp"
#include <glm/gtc/quaternion.hpp>

namespace WorldMaker {
    struct Feature {
        std::string name;
        std::string modelPath;
        double y_offset = 0;
        glm::vec3 scale = glm::vec3(1, 1, 1);
        glm::quat rotation = glm::identity<glm::quat>();
        int terrestrialStatus = 0;
        std::vector<int> radius;
        std::vector<double> probability;
    };

    class FeatureManager {
    public:
        static std::vector<Feature> m_features;
        static void addFeature(const Feature& feature);
        static void removeFeature(const int index);
        static int getFeatureId(const Feature& feature);
        static void addDefaultFeatures();
    };
}
