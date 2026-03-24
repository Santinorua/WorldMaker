#include "Interpolations.h"

namespace WorldMaker
{
    double Lerp(double a, double b, double t, bool useSmoothStep) {
        if (useSmoothStep) {
            t = SmoothStep(t);
        }
        return a + t * (b-a);
    }

}