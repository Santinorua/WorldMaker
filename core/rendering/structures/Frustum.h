#pragma once

#include "Plane.h"

namespace WorldMaker
{
    class Camera;
    struct Frustum
    {
        Frustum(); // Uses the static class Camera
        bool sphereInside(glm::vec3 center, float radius);
        bool boxInside(glm::vec3 min, glm::vec3 max);
        Plane nearPlane;
        Plane farPlane;
        Plane leftPlane;
        Plane rightPlane;
        Plane topPlane;
        Plane bottomPlane;
        Plane* planes[6] =
        {
            &nearPlane,
            &farPlane,
            &leftPlane,
            &rightPlane,
            &topPlane,
            &bottomPlane
        };
    private:
        void update();
        void check();
    };
}
