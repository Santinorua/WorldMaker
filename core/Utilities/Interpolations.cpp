#include "Interpolations.h"

namespace WorldMaker
{
    double Lerp(double a, double b, double t, bool useSmoothStep) {
        if (useSmoothStep) {
            t = SmoothStep(t);
        }
        return a + t * (b-a);
    }

    double Bilinear(Vec4 a, double x, double y) {
        double bottom = Lerp(a.x, a.y, x, true);
        double top = Lerp(a.z, a.w, x, true);
        return Lerp(bottom, top, y, true);
    }

}