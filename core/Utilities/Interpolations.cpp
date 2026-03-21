#include "Interpolations.h"

namespace WorldMaker
{
    double Lerp(double a, double b, double t, bool useSmoothStep) {
        if (useSmoothStep) {
            t = SmoothStep(t);
        }
        return a + t * (b-a);
    }

    double Bilinear(Vec4 x) {
        double top = Lerp(x.x, x.y, 0.5, true);
        double bottom = Lerp(x.z, x.w, 0.5, true);
        return Lerp(top, bottom, 0.5, true);
    }

}