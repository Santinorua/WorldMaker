#include "Frustum.h"

#include "Camera.h"

namespace WorldMaker
{
    Frustum::Frustum() {}
    void Frustum::check()
    {
        std::string state = "inside";
        glm::vec3 inside = Camera::Position()+Camera::Down()*10.0f+Camera::Front()*1.0f;

        float dot = glm::dot(leftPlane.normal, inside-leftPlane.point);
        state = dot>0 ? "inside" : "outside";
        std::cout << "Left plane says: " << state << "\n";

        dot = glm::dot(rightPlane.normal, inside-rightPlane.point);
        state = dot>0 ? "inside" : "outside";
        std::cout << "Right plane says: " << state << "\n";

        dot = glm::dot(topPlane.normal, inside-topPlane.point);
        state = dot>0 ? "inside" : "outside";
        std::cout << "Top plane says: " << state << "\n";

        dot = glm::dot(bottomPlane.normal, inside-bottomPlane.point);
        state = dot>0 ? "inside" : "outside";
        std::cout << "Bottom plane says: " << state << "\n";

        dot = glm::dot(farPlane.normal, inside-farPlane.point);
        state = dot>0 ? "inside" : "outside";
        std::cout << "Far plane says: " << state << "\n";

        dot = glm::dot(nearPlane.normal, inside-nearPlane.point);
        state = dot>0 ? "inside" : "outside";
        std::cout << "Near plane says: " << state << "\n";
    }
    void Frustum::update()
    {
        glm::vec3 nearPlanePoint = Camera::Position()+Camera::Front()*Camera::NearPlane();
        nearPlane = {nearPlanePoint, Camera::Front()};

        glm::vec3 farPlanePoint = Camera::Position()+Camera::Front()*Camera::FarPlane();
        farPlane = {farPlanePoint, Camera::Back()};

        float radiansHalfFOV = glm::radians(Camera::FOV()/2);
        float farPlaneHalfHeight = glm::tan(radiansHalfFOV) * Camera::FarPlane();
        float farPlaneHalfWidth = farPlaneHalfHeight * Camera::AspectRatio();

        glm::vec3 leftPlanePoint = farPlanePoint+farPlaneHalfWidth*Camera::Left();
        glm::vec3 leftPlaneNormal = glm::normalize(glm::cross(Camera::Up(), leftPlanePoint-Camera::Position()));
        leftPlane = { leftPlanePoint, leftPlaneNormal };

        glm::vec3 rightPlanePoint = farPlanePoint+farPlaneHalfWidth*Camera::Right();
        glm::vec3 rightPlaneNormal = glm::normalize(glm::cross(rightPlanePoint-Camera::Position(), Camera::Up()));
        rightPlane = {rightPlanePoint, rightPlaneNormal};

        glm::vec3 bottomPlanePoint = farPlanePoint+farPlaneHalfHeight*Camera::Down();
        glm::vec3 bottomPlaneNormal = glm::normalize(glm::cross(Camera::Left(), bottomPlanePoint-Camera::Position()));
        bottomPlane = { bottomPlanePoint, bottomPlaneNormal };

        glm::vec3 topPlanePoint = farPlanePoint+farPlaneHalfHeight*Camera::Up();
        glm::vec3 topPlaneNormal = glm::normalize(glm::cross(Camera::Right(), topPlanePoint-Camera::Position()));
        topPlane = {topPlanePoint, topPlaneNormal};
    }
    bool Frustum::sphereInside(glm::vec3 center, float radius)
    {
        update();

        for(const Plane* plane : planes)
        {
            float distance =
            glm::dot(
                plane->normal,
                center - plane->point
            );

            if(distance < -radius) return false;
        }

        return true;
    }
    bool Frustum::boxInside(glm::vec3 min, glm::vec3 max)
    {
        update();
        for (const Plane* plane : planes)
        {
            glm::vec3 point;
            point.x = plane->normal.x>0 ? max.x : min. x;
            point.y = plane->normal.y>0 ? max.y : min. y;
            point.z = plane->normal.z>0 ? max.z : min. z;

            float distance =
            glm::dot(
                plane->normal,
                point - plane->point
            );

            if (distance < 0) return false;
        }
        return true;
    }
}
