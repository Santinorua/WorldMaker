#pragma once
#include "Vec4.h"
#include <array>

namespace WorldMaker
{
    inline double SmoothStep(double x) {
        if (x < 0) return 0;
        if (x > 1) return 1;
        return ((6*x - 15)*x + 10)*x*x*x;
    }
    double Lerp(double a, double b, double t, bool useSmoothStep);
    double Bilinear(Vec4 a, double x, double y);
    double Trilinear(double a[8], double x, double y, double z);
}