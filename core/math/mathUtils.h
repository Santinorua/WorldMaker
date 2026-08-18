#pragma once
#include <cmath>

double moduloSqrt(double x) {
    if (x < 0) {
        return -std::sqrt(-x);
    } else {
        return std::sqrt(x);
    }
}