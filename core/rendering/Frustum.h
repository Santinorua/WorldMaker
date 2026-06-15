#pragma once

#include "Plane.h"

namespace WorldMaker
{
    class Camera;
    struct Frustum
    {
        Frustum(); // Uses the static class Camera
        bool sphereInside(glm::vec3 center, float radius);
        Plane nearPlane;
        Plane farPlane;
        Plane leftPlane;
        Plane rightPlane;
        Plane topPlane;
        Plane bottomPlane;
    private:
        void update();
        void check();
    };
}
