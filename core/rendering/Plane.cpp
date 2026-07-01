#include "Plane.h"

namespace WorldMaker
{
    Plane::Plane()
        : normal(0.0f), point(0.0f)
    {
    }

    Plane::Plane(glm::vec3 p_point, glm::vec3 p_normal)
        :normal(p_normal), point(p_point)
    {

    }
}
