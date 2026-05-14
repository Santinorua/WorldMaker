#pragma once
#include <vector>
#include "Vec2.h"
#include "Vec4.h"
#include "PRNG.h"
#include "Interpolations.h"
#include "cmath"
#include <array>

namespace WorldMaker {

    class PerlinNoise {
        uint64_t m_seed;
        double m_scale;
        int m_height;
        int m_width;

        double dotGradient(int x, int y, double sampleX, double sampleY);
    public:
        PerlinNoise(int width, int height, double frequency, uint64_t seed);
        double getPerlinNoise(float x, float y);
    };

    class PerlinNoise3D {
        uint64_t m_seed;
        double m_scale;
        double m_heightScale;
        int m_height;
        int m_width;

        double dotGradient3D(int x, int y, int z, double sampleX, double sampleY, double sampleZ);
    public:
        PerlinNoise3D(int width, int height, double frequency, double heightScale, uint64_t seed);
        double getPerlinNoise3D(float x, float y, float z);
    };

    class FractalNoise {
        double m_persistance;
        double m_amplitude;
        std::vector<PerlinNoise> m_octaves = {};

    public:
        FractalNoise(int width, int height, double frequency, double amplitude, uint64_t seed, int octaves, double lacunarity, double persistence);
        double getNoise(float x, float y);
    };



    class ComplexNoise {
        std::vector<PerlinNoise> m_octaves = {};
        std::vector<double> m_amplitudes = {};
    public:
        ComplexNoise(int width, int height, int first, const std::vector<double>& amplitudes, uint64_t seed);
        double getNoise(float x, float y);
    };

    class RidgesFolded {
        FractalNoise m_fractalNoise;
    public:
        RidgesFolded();
        double getNoise(float x, float y);
    };

}
