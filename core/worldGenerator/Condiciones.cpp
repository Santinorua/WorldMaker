#include "BiomeGenerator.h"
#include <functional>

namespace WorldMaker {
    std::function<bool(double)> MayorQue(double threshold) {
        return [threshold](const double &value) {
            return value > threshold;
        };
    }
    std::function<bool(double)> MayorOIgualQue(double threshold) {
        return [threshold](const double &value) {
            return value >= threshold;
        };
    }
    std::function<bool(double)> MenorQue(double threshold) {
        return [threshold](const double &value) {
            return value < threshold;
        };
    }
    std::function<bool(double)> MenorOIgualQue(double threshold) {
        return [threshold](const double &value) {
            return value <= threshold;
        };
    }
    std::function<bool(double)> Igual(double threshold) {
        return [threshold](const double &value) {
            return value == threshold;
        };
    }
    std::function<bool(double)> Distinto(double threshold) {
        return [threshold](const double &value) {
            return value != threshold;
        };
    }
}