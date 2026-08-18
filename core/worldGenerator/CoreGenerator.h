#pragma once

#include <cstdint>

#include "BiomeGenerator.h"
#include "PerlinNoise.h"
#include "Vertex.h"

namespace WorldMaker {
    struct generatorVertex {
        Vec3 m_position;  // x, y, z
        Vec4 m_color = {1,1,1,1};
        double m_materialLayer = 0;
        Vec2 m_uv;
        Vec3 m_normal;

        uint16_t m_featureId = 0;

        operator std::vector<double>() const
        {
            return
            {
                m_position.x, m_position.y, m_position.z,
                m_color.x, m_color.y, m_color.z, m_color.y,
                m_materialLayer,
                m_uv.x, m_uv.y,
                m_normal.x, m_normal.y, m_normal.z
            };
        }
        Vertex toGraphicalVertex() const {
            Vertex vertex;
            vertex.m_position = m_position;
            vertex.m_color = m_color;
            vertex.m_materialLayer = m_materialLayer;
            vertex.m_uv = m_uv;
            vertex.m_normal = m_normal;
            return vertex;
        }
    };

    class WorldGenerator {
    public:
        uint64_t m_seed;
        double m_yScale;
        FractalNoise m_erosion;
        FractalNoise m_continentalness;
        FractalNoise m_temperature;
        FractalNoise m_humidity;
        FractalNoise m_base;
        uint64_t m_featureSeed;
    public:
        double getHeight(double erosion, double continentalness, double base);
        generatorVertex getVertex(float x, float z);
        WorldGenerator(double yScale, uint64_t seed);
    };


}
