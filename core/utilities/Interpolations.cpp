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
    double Trilinear(double a[8], double x, double y, double z) {
        double x00 = Lerp(a[0], a[1], x, true);
        double x10 = Lerp(a[2], a[3], x, true);
        double x01 = Lerp(a[4], a[5], x, true);
        double x11 = Lerp(a[6], a[7], x, true);

        double y0 = Lerp(x00, x10, y, true);
        double y1 = Lerp(x01, x11, y, true);

        return Lerp(y0, y1, z, true);
    }

}
