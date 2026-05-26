#include "BiomeGenerator.h"
#include <functional>

namespace WorldMaker {
    std::function<bool> MayorQue(double threshold) {
        return [threshold](const double &value) {
            return value > threshold;
        };
    }
    std::function<bool> MayorOIgualQue(double threshold) {
        return [threshold](const double &value) {
            return value >= threshold;
        };
    }
    std::function<bool> MenorQue(double threshold) {
        return [threshold](const double &value) {
            return value < threshold;
        };
    }
    std::function<bool> MenorOIgualQue(double threshold) {
        return [threshold](const double &value) {
            return value <= threshold;
        };
    }
    std::function<bool> Igual(double threshold) {
        return [threshold](const double &value) {
            return value == threshold;
        };
    }
    std::function<bool> Distinto(double threshold) {
        return [threshold](const double &value) {
            return value != threshold;
        };
    }
}