#pragma once

namespace WorldMaker
{
    inline double SmoothStep(double x) {
        return x * x * 3 - x * x * x * 2;
    }
    double Lerp(double a, double b, double t, bool useSmoothStep);

}