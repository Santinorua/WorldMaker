#pragma once

namespace WorldMaker
{
    inline double dLerp(double a, double b, double t) {
        return a + t * (b - a);
    }
}